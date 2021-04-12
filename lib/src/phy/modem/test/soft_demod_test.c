/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/srsran.h"

static uint32_t     nof_frames = 10;
static uint32_t     num_bits   = 1000;
static srsran_mod_t modulation = SRSRAN_MOD_NITEMS;

static void usage(char* prog)
{
  printf("Usage: %s [nfv] -m modulation (1: BPSK, 2: QPSK, 4: QAM16, 6: QAM64)\n", prog);
  printf("\t-n num_bits [Default %d]\n", num_bits);
  printf("\t-f nof_frames [Default %d]\n", nof_frames);
  printf("\t-v srsran_verbose [Default None]\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "nmvf")) != -1) {
    switch (opt) {
      case 'n':
        num_bits = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        nof_frames = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srsran_verbose++;
        break;
      case 'm':
        switch (strtol(argv[optind], NULL, 10)) {
          case 1:
            modulation = SRSRAN_MOD_BPSK;
            break;
          case 2:
            modulation = SRSRAN_MOD_QPSK;
            break;
          case 4:
            modulation = SRSRAN_MOD_16QAM;
            break;
          case 6:
            modulation = SRSRAN_MOD_64QAM;
            break;
          case 8:
            modulation = SRSRAN_MOD_256QAM;
            break;
          default:
            ERROR("Invalid modulation %d. Possible values: "
                  "(1: BPSK, 2: QPSK, 4: QAM16, 6: QAM64)",
                  (int)strtol(argv[optind], NULL, 10));
            break;
        }
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (modulation == SRSRAN_MOD_NITEMS) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  srsran_modem_table_t mod;
  uint8_t*             input      = NULL;
  cf_t*                symbols    = NULL;
  float*               llr        = NULL;
  short*               llr_s      = NULL;
  int8_t*              llr_b      = NULL;
  int8_t*              llr_b2     = NULL;
  srsran_random_t      random_gen = srsran_random_init(0);

  parse_args(argc, argv);

  /* initialize objects */
  if (srsran_modem_table_lte(&mod, modulation)) {
    ERROR("Error initializing modem table");
    exit(-1);
  }

  /* check that num_bits is multiple of num_bits x symbol */
  num_bits = mod.nbits_x_symbol * (num_bits / mod.nbits_x_symbol);

  /* allocate buffers */
  input = srsran_vec_u8_malloc(num_bits);
  if (!input) {
    perror("malloc");
    exit(-1);
  }
  symbols = srsran_vec_cf_malloc(num_bits / mod.nbits_x_symbol);
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }

  llr = srsran_vec_f_malloc(num_bits);
  if (!llr) {
    perror("malloc");
    exit(-1);
  }

  llr_s = srsran_vec_i16_malloc(num_bits);
  if (!llr_s) {
    perror("malloc");
    exit(-1);
  }

  llr_b = srsran_vec_i8_malloc(num_bits);
  if (!llr_b) {
    perror("malloc");
    exit(-1);
  }

  llr_b2 = srsran_vec_i8_malloc(num_bits);
  if (!llr_b2) {
    perror("malloc");
    exit(-1);
  }

  int            ret = -1;
  struct timeval t[3];
  float          mean_texec    = 0.0f;
  float          mean_texec_s  = 0.0f;
  float          mean_texec_b  = 0.0f;
  float          mean_texec_b2 = 0.0f;
  for (int n = 0; n < nof_frames; n++) {
    for (int i = 0; i < num_bits; i++) {
      input[i] = srsran_random_uniform_int_dist(random_gen, 0, 1);
    }

    /* modulate */
    srsran_mod_modulate(&mod, input, symbols, num_bits);

    gettimeofday(&t[1], NULL);
    srsran_demod_soft_demodulate(modulation, symbols, llr, num_bits / mod.nbits_x_symbol);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);

    /* compute exponentially averaged execution time */
    if (n > 0) {
      mean_texec = SRSRAN_VEC_CMA((float)t[0].tv_usec, mean_texec, n - 1);
    }

    gettimeofday(&t[1], NULL);
    srsran_demod_soft_demodulate_s(modulation, symbols, llr_s, num_bits / mod.nbits_x_symbol);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);

    if (n > 0) {
      mean_texec_s = SRSRAN_VEC_CMA((float)t[0].tv_usec, mean_texec_s, n - 1);
    }

    gettimeofday(&t[1], NULL);
    srsran_demod_soft_demodulate_b(modulation, symbols, llr_b, num_bits / mod.nbits_x_symbol);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);

    if (n > 0) {
      mean_texec_b = SRSRAN_VEC_CMA((float)t[0].tv_usec, mean_texec_b, n - 1);
    }

    gettimeofday(&t[1], NULL);
    srsran_demod_soft_demodulate2_b(modulation, symbols, llr_b2, num_bits / mod.nbits_x_symbol);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);

    if (n > 0) {
      mean_texec_b2 = SRSRAN_VEC_CMA((float)t[0].tv_usec, mean_texec_b2, n - 1);
    }

    if (SRSRAN_VERBOSE_ISDEBUG()) {
      printf("bits=");
      srsran_vec_fprint_b(stdout, input, num_bits);

      printf("symbols=");
      srsran_vec_fprint_c(stdout, symbols, num_bits / mod.nbits_x_symbol);

      printf("llr=");
      srsran_vec_fprint_f(stdout, llr, num_bits);

      printf("llr_s=");
      srsran_vec_fprint_s(stdout, llr_s, num_bits);

      printf("llr_b=");
      srsran_vec_fprint_bs(stdout, llr_b, num_bits);

      printf("llr_b2=");
      srsran_vec_fprint_bs(stdout, llr_b2, num_bits);
    }

    // Check demodulation errors
    for (int j = 0; j < num_bits; j++) {
      if (input[j] != (llr[j] > 0 ? 1 : 0)) {
        ERROR("Error in bit %d\n", j);
        goto clean_exit;
      }
      if (input[j] != (llr_s[j] > 0 ? 1 : 0)) {
        ERROR("Error in bit %d\n", j);
        goto clean_exit;
      }
      if (input[j] != (llr_b[j] > 0 ? 1 : 0)) {
        ERROR("Error in bit %d\n", j);
        goto clean_exit;
      }
      if (input[j] != (llr_b2[j] > 0 ? 1 : 0)) {
        ERROR("Error in bit %d\n", j);
        goto clean_exit;
      }
    }
  }
  ret = 0;

clean_exit:
  srsran_random_free(random_gen);
  free(llr_b);
  free(llr_s);
  free(llr);
  free(symbols);
  free(input);

  srsran_modem_table_free(&mod);

  printf("Mean Throughput: %.2f/%.2f/%.2f/%.2f. Mbps ExTime: %.2f/%.2f/%.2f/%.2f us\n",
         num_bits / mean_texec,
         num_bits / mean_texec_s,
         num_bits / mean_texec_b,
         num_bits / mean_texec_b2,
         mean_texec,
         mean_texec_s,
         mean_texec_b,
         mean_texec_b2);
  exit(ret);
}
