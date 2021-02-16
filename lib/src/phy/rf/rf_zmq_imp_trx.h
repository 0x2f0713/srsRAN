/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_RF_ZMQ_IMP_TRX_H
#define SRSLTE_RF_ZMQ_IMP_TRX_H

#include <pthread.h>
#include <srslte/phy/utils/ringbuffer.h>
#include <stdbool.h>

/* Definitions */
#define VERBOSE (0)
#define ZMQ_MONITOR (0)
#define NSAMPLES2NBYTES(X) (((uint32_t)(X)) * sizeof(cf_t))
#define NBYTES2NSAMPLES(X) ((X) / sizeof(cf_t))
#define ZMQ_MAX_BUFFER_SIZE (NSAMPLES2NBYTES(3072000)) // 10 subframes at 20 MHz
#define ZMQ_TIMEOUT_MS (2000)
#define ZMQ_BASERATE_DEFAULT_HZ (23040000)
#define ZMQ_ID_STRLEN 16
#define ZMQ_MAX_GAIN_DB (30.0f)
#define ZMQ_MIN_GAIN_DB (0.0f)

typedef enum { ZMQ_TYPE_FC32 = 0, ZMQ_TYPE_SC16 } rf_zmq_format_t;

typedef struct {
  char            id[ZMQ_ID_STRLEN];
  uint32_t        socket_type;
  rf_zmq_format_t sample_format;
  void*           sock;
  uint64_t        nsamples;
  bool            running;
  pthread_mutex_t mutex;
  cf_t*           zeros;
  void*           temp_buffer_convert;
  uint32_t        frequency_mhz;
  int32_t         sample_offset;
} rf_zmq_tx_t;

typedef struct {
  char            id[ZMQ_ID_STRLEN];
  uint32_t        socket_type;
  rf_zmq_format_t sample_format;
  void*           sock;
#if ZMQ_MONITOR
  void* socket_monitor;
  bool  tx_connected;
#endif
  uint64_t            nsamples;
  bool                running;
  pthread_t           thread;
  pthread_mutex_t     mutex;
  srslte_ringbuffer_t ringbuffer;
  cf_t*               temp_buffer;
  void*               temp_buffer_convert;
  uint32_t            frequency_mhz;
  bool                fail_on_disconnect;
  uint32_t            trx_timeout_ms;
  bool                log_trx_timeout;
  int32_t             sample_offset;
} rf_zmq_rx_t;

typedef struct {
  const char*     id;
  uint32_t        socket_type;
  rf_zmq_format_t sample_format;
  uint32_t        frequency_mhz;
  bool            fail_on_disconnect;
  uint32_t        trx_timeout_ms;
  bool            log_trx_timeout;
  int32_t         sample_offset; ///< offset in samples
} rf_zmq_opts_t;

/*
 * Common functions
 */
SRSLTE_API void rf_zmq_info(char* id, const char* format, ...);

SRSLTE_API void rf_zmq_error(char* id, const char* format, ...);

SRSLTE_API int rf_zmq_handle_error(char* id, const char* text);

/*
 * Transmitter functions
 */
SRSLTE_API int rf_zmq_tx_open(rf_zmq_tx_t* q, rf_zmq_opts_t opts, void* zmq_ctx, char* sock_args);

SRSLTE_API int rf_zmq_tx_align(rf_zmq_tx_t* q, uint64_t ts);

SRSLTE_API int rf_zmq_tx_baseband(rf_zmq_tx_t* q, cf_t* buffer, uint32_t nsamples);

SRSLTE_API int rf_zmq_tx_zeros(rf_zmq_tx_t* q, uint32_t nsamples);

SRSLTE_API bool rf_zmq_tx_match_freq(rf_zmq_tx_t* q, uint32_t freq_hz);

SRSLTE_API void rf_zmq_tx_close(rf_zmq_tx_t* q);

/*
 * Receiver functions
 */
SRSLTE_API int rf_zmq_rx_open(rf_zmq_rx_t* q, rf_zmq_opts_t opts, void* zmq_ctx, char* sock_args);

SRSLTE_API int rf_zmq_rx_baseband(rf_zmq_rx_t* q, cf_t* buffer, uint32_t nsamples);

SRSLTE_API bool rf_zmq_rx_match_freq(rf_zmq_rx_t* q, uint32_t freq_hz);

SRSLTE_API void rf_zmq_rx_close(rf_zmq_rx_t* q);

#endif // SRSLTE_RF_ZMQ_IMP_TRX_H
