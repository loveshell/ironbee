/*****************************************************************************
 * Licensed to Qualys, Inc. (QUALYS) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * QUALYS licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 * @brief IronBee &mdash; BinRadix Matcher Module
 *
 * This module adds a IP BinRadix based matcher named "binradix".
 *
 * @author Pablo Rincon <pablo.rincon.crespo@gmail.com>
 */

#include <ironbee/debug.h>
#include <ironbee/engine.h>
#include <ironbee/module.h>
#include <ironbee/mpool.h>
#include <ironbee/provider.h>
#include <ironbee/radix.h>

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

/* Define the module name as well as a string version of it. */
#define MODULE_NAME        binradix
#define MODULE_NAME_STR    IB_XSTRINGIFY(MODULE_NAME)

/* Informational extra data.. version of this module (should be better to
 * register it with the module itself) */
#define AC_MAJOR           0
#define AC_MINOR           1

typedef struct modbinradix_content_t modbinradix_content_t;
typedef ib_status_t (*modbinradix_callback_t)(void *);

/* Instantiate a module global configuration. */
typedef struct modbinradix_provider_data_t modbinradix_provider_data_t;

/* Define the public module symbol. */
IB_MODULE_DECLARE();

/**
 * Internal structure for storing prefixes
 */
struct modbinradix_provider_data_t {
    ib_radix_t *binradix_tree;        /**< The BinRadix tree */
};

/**
 * Content for a prefix.
 *
 * This content will be associated to the registered prefix instances
 * If a match occur and data != NULL, this callback will be called.
 * If data is NULL the match() function will be considered as if
 * no match happened. The reason is that we can implement here
 * "exceptions" of certain ip addresses/ranges inside of a registered
 * subnet. If you do not need to pass any extra data, just set it to 1
 * and ignore it as a pointer in the callback (if any)
 */
struct modbinradix_content_t {
    void *data;
    modbinradix_callback_t callback;  /**< Callback to call if a prefix match */
};


/* -- Matcher Interface -- */

/**
 * Add a prefix to the prefixes of the binradix, given a prefix and
 * callback + extra arg
 *
 * @param mpi matcher provider
 * @param prefixes pointer to the prefix container (i.e.: an BinRadix tree)
 * @param prefix the prefix to be added
 * @param callback the callback to register with the given prefix
 * @param arg the extra argument to pass to the callback
 * @param errptr a pointer reference to point where an error occurred
 * @param erroffset a pointer holding the offset of the error
 *
 * @return status of the operation
 */
static ib_status_t modbinradix_add_prefix_ex(ib_provider_inst_t *mpi,
                                             void *prefixes,
                                             const char *prefix,
                                             ib_void_fn_t callback,
                                             void *arg,
                                             const char **errptr,
                                             int *erroffset)
{
    IB_FTRACE_INIT();
    ib_status_t rc;
    ib_radix_t *binradix_tree = (ib_radix_t *)mpi->data;

    modbinradix_content_t *mrc = NULL;
    mrc = (modbinradix_content_t *)ib_mpool_calloc(mpi->pr->mp, 1,
                                               sizeof(modbinradix_content_t));
    if (mrc == NULL) {
        ib_log_error(mpi->pr->ib,  "Failed to allocate modbinradix_content_t"
                                 " for %s to the BinRadix tree %p", prefix,
                                 binradix_tree);
        IB_FTRACE_RET_STATUS(IB_EALLOC);
    }

    mrc->data = arg;
    mrc->callback = (modbinradix_callback_t)callback;

    ib_radix_prefix_t *pre = NULL;

    rc = ib_radix_ip_to_prefix(prefix, &pre, mpi->mp);
    if (rc != IB_OK) {
        ib_log_error(mpi->pr->ib,  "Failed to create a binradix prefix for %s"
                                 " to the BinRadix tree %p", prefix,
                                 binradix_tree);
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = ib_radix_insert_data(binradix_tree, pre, (void *) mrc);

    if (rc == IB_OK) {
        ib_log_debug(mpi->pr->ib, "prefix %s added to the BinRadix tree %p",
                     prefix, binradix_tree);
    }
    else {
        ib_log_error(mpi->pr->ib,  "Failed to load prefix %s to the BinRadix "
                                 "tree %p", prefix, binradix_tree);
    }

    IB_FTRACE_RET_STATUS(rc);
}

/**
 * Initialize a provider instance with the given data
 *
 * @param mpi provider instance
 * @param data data
 *
 * @return status of the operation
 */
static ib_status_t modbinradix_provider_instance_init(ib_provider_inst_t *mpi,
                                                void *data)
{
    IB_FTRACE_INIT();
    modbinradix_provider_data_t *dt = NULL;
    ib_status_t rc;

    dt = (modbinradix_provider_data_t *) ib_mpool_calloc(mpi->mp, 1,
                                         sizeof(modbinradix_provider_data_t));
    if (dt == NULL) {
        IB_FTRACE_RET_STATUS(IB_EALLOC);
    }

    rc = ib_radix_new(&dt->binradix_tree, NULL, NULL, NULL, mpi->mp);
    if (rc != IB_OK) {
        ib_log_error(mpi->pr->ib,  "Unable to create the BinRadix tree at"
                                 " modbinradix");
        IB_FTRACE_RET_STATUS(rc);
    }

    mpi->data = (void *)dt;

    IB_FTRACE_RET_STATUS(IB_OK);
}

/**
 * Match against the BinRadix tree considering data as a binary IP address
 * This is the main difference with the other radix matcher (where data is
 * considered ascii)
 *
 * @param mpi provider instance
 * @param flags extra flags
 * @param data the data to search in
 * @param dlen length of the the data to search in
 * @param ctx Current configuration context
 *
 * @return status of the operation
 */
static ib_status_t modbinradix_match(ib_provider_inst_t *mpi,
                                 ib_flags_t flags,
                                 const uint8_t *data,
                                 size_t dlen,
                                 void *ctx)
{
    IB_FTRACE_INIT();
    ib_status_t rc;
    modbinradix_provider_data_t *dt = mpi->data;

    if (dt == NULL) {
        IB_FTRACE_RET_STATUS(IB_EINVAL);
    }

    ib_log_debug(mpi->pr->ib, "Matching AGAINST BinRadix tree %p",
                     dt->binradix_tree);

    ib_radix_t *binradix_tree = dt->binradix_tree;

    ib_radix_prefix_t *pre = NULL;

    /* Create the prefix directly. Data should be a binary ip address already */
    rc = ib_radix_prefix_create(&pre, (uint8_t *)data, (uint8_t)dlen * 8,
                                mpi->mp);
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    void *result = NULL;

    rc = ib_radix_match_closest(binradix_tree, pre, &result);
    if (rc == IB_OK) {
        modbinradix_content_t *mrc = (modbinradix_content_t *)result;
        if (mrc->callback != NULL && mrc->data != NULL) {
            *(void **)ctx = result;
            IB_FTRACE_RET_STATUS(mrc->callback(mrc->data));
        }
        else if (mrc->data != NULL) {
            *(void **)ctx = result;
            IB_FTRACE_RET_STATUS(IB_OK);
        }
        else {
            IB_FTRACE_RET_STATUS(IB_ENOENT);
        }
    }

    IB_FTRACE_RET_STATUS(rc);
}

static ib_status_t modbinradix_compile(ib_provider_t *mpr,
                                   ib_mpool_t *pool,
                                   void *pcprefix,
                                   const char *prefix,
                                   const char **errptr,
                                   int *erroffset)
{
    IB_FTRACE_INIT();
    IB_FTRACE_RET_STATUS(IB_ENOTIMPL);
}

static ib_status_t modbinradix_match_compiled(ib_provider_t *mpr,
                                          void *cprefix,
                                          ib_flags_t flags,
                                          const uint8_t *data,
                                          size_t dlen,
                                          void *ctx)
{
    IB_FTRACE_INIT();
    IB_FTRACE_RET_STATUS(IB_ENOTIMPL);
}

static ib_status_t modbinradix_add_prefix(ib_provider_inst_t *pi,
                                       void *cprefix)
{
    IB_FTRACE_INIT();
    IB_FTRACE_RET_STATUS(IB_ENOTIMPL);
}

static IB_PROVIDER_IFACE_TYPE(matcher) modbinradix_matcher_iface = {
    IB_PROVIDER_IFACE_HEADER_DEFAULTS,

    /* Provider Interface */
    modbinradix_compile,
    modbinradix_match_compiled,

    /* Provider Instance Interface */
    modbinradix_add_prefix,
    modbinradix_add_prefix_ex,
    modbinradix_match
};


/* -- Module Routines -- */

static ib_status_t modbinradix_init(ib_engine_t *ib,
                                    ib_module_t *m,
                                    void        *cbdata)
{
    IB_FTRACE_INIT();
    ib_status_t rc;

    /* Register as a matcher provider. */
    rc = ib_provider_register(ib,
                              IB_PROVIDER_TYPE_MATCHER,
                              MODULE_NAME_STR,
                              NULL,
                              &modbinradix_matcher_iface,
                              modbinradix_provider_instance_init);
    if (rc != IB_OK) {
        ib_log_error(ib,
                     MODULE_NAME_STR ": Error registering ac matcher provider: "
                     "%s", ib_status_to_string(rc));
        IB_FTRACE_RET_STATUS(IB_OK);
    }

    ib_log_debug(ib, "AC Status: compiled=\"%d.%d %s\" BinRadix Matcher"
                        " registered", AC_MAJOR, AC_MINOR,
                        IB_XSTRINGIFY(AC_DATE));

    IB_FTRACE_RET_STATUS(IB_OK);
}

/**
 * Module structure.
 *
 * This structure defines some metadata, config data and various functions.
 */
IB_MODULE_INIT(
    IB_MODULE_HEADER_DEFAULTS,              /**< Default metadata */
    MODULE_NAME_STR,                        /**< Module name */
    IB_MODULE_CONFIG_NULL,                  /**< Global config data */
    NULL,                                   /**< Configuration field map */
    NULL,                                   /**< Config directive map */
    modbinradix_init,                       /**< Initialize function */
    NULL,                                   /**< Callback data */
    NULL,                                   /**< Finish function */
    NULL,                                   /**< Callback data */
    NULL,                                   /**< Context open function */
    NULL,                                   /**< Callback data */
    NULL,                                   /**< Context close function */
    NULL,                                   /**< Callback data */
    NULL,                                   /**< Context destroy function */
    NULL                                    /**< Callback data */
);
