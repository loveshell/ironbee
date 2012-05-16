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
 * @brief IronBee - Core Module Fields
 *
 * @author Brian Rectanus <brectanus@qualys.com>
 */

#include "ironbee_config_auto.h"

#include <ironbee/debug.h>
#include <ironbee/core.h>
#include <ironbee_private.h>

#include <assert.h>


/* -- Field Generation Routines -- */

/* Placeholder for as-of-yet-initialized bytestring fields. */
static const uint8_t core_placeholder_value[] = {
    '_', '_', 'c', 'o', 'r', 'e', '_', '_',
    'p', 'l', 'a', 'c', 'e', 'h', 'o', 'l',
    'd', 'e', 'r', '_', '_', 'v', 'a', 'l',
    'u', 'e', '_', '_',  0,   0,   0,   0
};

static ib_status_t core_field_placeholder_bytestr(ib_provider_inst_t *dpi,
                                                  const char *name)
{
    IB_FTRACE_INIT();
    ib_status_t rc = ib_data_add_bytestr_ex(dpi,
                                            (const char *)name,
                                            strlen(name),
                                            (uint8_t *)core_placeholder_value,
                                            0,
                                            NULL);
    if (rc != IB_OK) {
        ib_log_error(dpi->pr->ib,
                     "Failed to generate \"%s\" placeholder field: %s",
                     name, ib_status_to_string(rc));
    }

    IB_FTRACE_RET_STATUS(rc);
}

static inline void core_gen_bytestr_alias_field(ib_tx_t *tx,
                                                const char *name,
                                                ib_bytestr_t *val)
{
    ib_field_t *f;

    assert(tx != NULL);
    assert(name != NULL);
    assert(val != NULL);

    ib_status_t rc = ib_field_create_no_copy(&f, tx->mp,
                                             name, strlen(name),
                                             IB_FTYPE_BYTESTR,
                                             val);
    if (rc != IB_OK) {
        ib_log_warning(tx->ib, "Failed to create \"%s\" field: %s",
                     name, ib_status_to_string(rc));
        return;
    }

    ib_log_debug(tx->ib, "FIELD: \"%s\"=\"%.*s\"",
                 name,
                 (int)ib_bytestr_length(val),
                 (char *)ib_bytestr_const_ptr(val));

    rc = ib_data_add(tx->dpi, f);
    if (rc != IB_OK) {
        ib_log_warning(tx->ib, "Failed add \"%s\" field to data store: %s",
                       name, ib_status_to_string(rc));
    }
}


/* -- Hooks -- */

static ib_status_t core_gen_placeholder_fields(ib_engine_t *ib,
                                               ib_tx_t *tx,
                                               ib_state_event_type_t event,
                                               void *cbdata)
{
    IB_FTRACE_INIT();

    assert(ib != NULL);
    assert(tx != NULL);
    assert(tx->dpi != NULL);
    assert(event == tx_started_event);

    ib_status_t rc;

    /* Core Request Fields */
    rc = ib_data_add_stream(tx->dpi, "request_body", NULL);
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_line");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_method");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_protocol");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_raw");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_scheme");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_username");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_password");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_host");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_host");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_port");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_path");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_query");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "request_uri_fragment");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    /* Core Request Collections */
    rc = ib_data_add_list(tx->dpi, "request_headers", NULL);
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = ib_data_add_list(tx->dpi, "request_cookies", NULL);
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = ib_data_add_list(tx->dpi, "request_uri_params", NULL);
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    /* Core Response Fields */
    rc = ib_data_add_stream(tx->dpi, "response_body", NULL);
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "response_line");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "response_protocol");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "response_status");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    rc = core_field_placeholder_bytestr(tx->dpi, "response_message");
    if (rc != IB_OK) {
        IB_FTRACE_RET_STATUS(rc);
    }

    /* Core Response Collections */
    rc = ib_data_add_list(tx->dpi, "response_headers", NULL);

    IB_FTRACE_RET_STATUS(rc);
}

/*
 * Callback used to generate request fields.
 */
static ib_status_t core_gen_request_header_fields(ib_engine_t *ib,
                                                  ib_tx_t *tx,
                                                  ib_state_event_type_t event,
                                                  void *cbdata)
{
    IB_FTRACE_INIT();

    assert(ib != NULL);
    assert(tx != NULL);
    assert(event == handle_context_tx_event);

    ib_log_debug(ib, "core_gen_request_header_fields");

    core_gen_bytestr_alias_field(tx, "request_line",
                                 tx->request_line->raw);

    core_gen_bytestr_alias_field(tx, "request_method",
                                 tx->request_line->method);

    core_gen_bytestr_alias_field(tx, "request_uri_raw",
                                 tx->request_line->uri);

    core_gen_bytestr_alias_field(tx, "request_protocol",
                                 tx->request_line->protocol);

    IB_FTRACE_RET_STATUS(IB_OK);
}

/*
 * Callback used to generate response fields.
 */
static ib_status_t core_gen_response_header_fields(ib_engine_t *ib,
                                                   ib_tx_t *tx,
                                                   ib_state_event_type_t event,
                                                   void *cbdata)
{
    IB_FTRACE_INIT();

    assert(ib != NULL);
    assert(tx != NULL);
    assert(event == response_header_finished_event);

    ib_log_debug(ib, "core_gen_response_header_fields");

    if (tx->response_line != NULL) {
        core_gen_bytestr_alias_field(tx, "response_line",
                                     tx->response_line->raw);

        core_gen_bytestr_alias_field(tx, "response_protocol",
                                     tx->response_line->protocol);

        core_gen_bytestr_alias_field(tx, "response_status",
                                     tx->response_line->status);

        core_gen_bytestr_alias_field(tx, "response_message",
                                     tx->response_line->msg);
    }

    IB_FTRACE_RET_STATUS(IB_OK);
}


/* -- Initialization Routines -- */

/* Initialize libhtp config object for the context. */
ib_status_t ib_core_fields_ctx_init(ib_engine_t *ib,
                                    ib_module_t *mod,
                                    ib_context_t *ctx,
                                    void *cbdata)
{
    IB_FTRACE_INIT();
    ib_core_cfg_t *corecfg;
    ib_status_t rc;

    assert(ib != NULL);
    assert(mod != NULL);
    assert(ctx != NULL);

    /* Get the core context config. */
    rc = ib_context_module_config(ctx, mod, (void *)&corecfg);
    if (rc != IB_OK) {
        ib_log_alert(ib,
                     "Failed to fetch core module context config: %s", ib_status_to_string(rc));
        IB_FTRACE_RET_STATUS(rc);
    }

    IB_FTRACE_RET_STATUS(IB_OK);
}

/* Initialize core field generation callbacks. */
ib_status_t ib_core_fields_init(ib_engine_t *ib,
                                ib_module_t *mod)
{
    IB_FTRACE_INIT();

    assert(ib != NULL);
    assert(mod != NULL);


    ib_hook_tx_register(ib, tx_started_event,
                        core_gen_placeholder_fields, NULL);

    ib_hook_tx_register(ib, handle_context_tx_event,
                        core_gen_request_header_fields, NULL);

    ib_hook_tx_register(ib, response_header_finished_event,
                        core_gen_response_header_fields, NULL);

    IB_FTRACE_RET_STATUS(IB_OK);
}
