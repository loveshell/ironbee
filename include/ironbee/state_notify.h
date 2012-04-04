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

#ifndef _IB_STATE_NOTIFY_H_
#define _IB_STATE_NOTIFY_H_

#include <ironbee/engine.h>
#include <ironbee/types.h>

#ifdef __cplusplus
extern "C" {
#endif


/* File tag is intentionally omitted. */
/**
 * @brief IronBee interface for handling parsed content.
 *
 * @author Sam Baskinger <sbaskinger@qualys.com>
 *
 * @ingroup IronBeeEngineState State
 * API For passing parsed or partially parsed content to the IronBee Engine.
 * @{
 */

/**
 * Notify the state machine that the configuration process has started.
 *
 * @param ib Engine handle
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_cfg_started(ib_engine_t *ib);

/**
 * Notify the state machine that the configuration process has finished.
 *
 * @param ib Engine handle
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_cfg_finished(ib_engine_t *ib);

/**
 * Notify the state machine that a connection started.
 *
 * @param ib Engine handle
 * @param conn Connection
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_conn_opened(ib_engine_t *ib,
                                                   ib_conn_t *conn);

/**
 * Notify the state machine that connection data came in.
 *
 * @param ib Engine handle
 * @param conndata Connection data
 * @param ctx Data to pass back to callback functions
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_conn_data_in(ib_engine_t *ib,
                                                    ib_conndata_t *conndata,
                                                    void *ctx);

/**
 * Notify the state machine that connection data is headed out.
 *
 * @param ib Engine handle
 * @param conndata Connection data
 * @param ctx Data to pass back to callback functions
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_conn_data_out(ib_engine_t *ib,
                                                     ib_conndata_t *conndata,
                                                     void *ctx);

/**
 * Notify the state machine that a connection finished.
 *
 * @param ib Engine handle
 * @param conn Connection
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_conn_closed(ib_engine_t *ib,
                                                   ib_conn_t *conn);

/**
 * Notify the state machine that transaction data came in.
 *
 * @param ib Engine handle
 * @param txdata Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_tx_data_in(ib_engine_t *ib,
                                                  ib_txdata_t *txdata);

/**
 * Notify the state machine that transaction data is headed out.
 *
 * @param ib Engine handle
 * @param txdata Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_tx_data_out(ib_engine_t *ib,
                                                   ib_txdata_t *txdata);

/**
 * Notify the state machine that the request started.
 *
 * @note This is an optional event. Unless the plugin can detect that
 *       a request has started prior to receiving the headers, then you
 *       should just call @ref ib_state_notify_request_headers() when the
 *       headers are received which will automatically notify that the
 *       request has started.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_request_started(ib_engine_t *ib,
                                                       ib_tx_t *tx);

/**
 * Notify the state machine that request headers are available.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_request_headers(ib_engine_t *ib,
                                                       ib_tx_t *tx);

/**
 * Notify the state machine that the request body is available.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_request_body(ib_engine_t *ib,
                                                    ib_tx_t *tx);

/**
 * Notify the state machine that a request finished.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_request_finished(ib_engine_t *ib,
                                                        ib_tx_t *tx);

/**
 * Notify the state machine that a response started.
 *
 * @note This is an optional event. Unless the plugin can detect that
 *       a response has started prior to receiving the headers, then you
 *       should just call @ref ib_state_notify_response_headers() when the
 *       headers are received which will automatically notify that the
 *       request has started.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_response_started(ib_engine_t *ib,
                                                        ib_tx_t *tx);

/**
 * Notify the state machine that the response headers are available.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_response_headers(ib_engine_t *ib,
                                                        ib_tx_t *tx);

/**
 * Notify the state machine that the response body is available.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_response_body(ib_engine_t *ib,
                                                     ib_tx_t *tx);

/**
 * Notify the state machine that a response finished.
 *
 * @param ib Engine handle
 * @param tx Transaction data
 *
 * @returns Status code
 */
ib_status_t DLL_PUBLIC ib_state_notify_response_finished(ib_engine_t *ib,
                                                         ib_tx_t *tx);


/**
 * @} IronBeeEngineState
 */
#ifdef __cplusplus
}
#endif
#endif