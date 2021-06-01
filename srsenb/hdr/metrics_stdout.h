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

/******************************************************************************
 * File:        metrics_stdout.h
 * Description: Metrics class printing to stdout.
 *****************************************************************************/

#ifndef SRSENB_METRICS_STDOUT_H
#define SRSENB_METRICS_STDOUT_H

#include <pthread.h>
#include <stdint.h>
#include <string>

#include "srsran/interfaces/enb_metrics_interface.h"

namespace srsenb {

class metrics_stdout : public srsran::metrics_listener<enb_metrics_t>
{
public:
  metrics_stdout();

  void toggle_print(bool b);
  void set_metrics(const enb_metrics_t& m, const uint32_t period_usec);
  void set_handle(enb_metrics_interface* enb_);
  void stop(){};

private:
  std::string float_to_string(float f, int digits, int field_width = 6);
  std::string int_to_hex_string(int value, int field_width);
  std::string float_to_eng_string(float f, int digits);

  std::atomic<bool>      do_print  = {false};
  uint8_t                n_reports = 0;
  enb_metrics_interface* enb       = nullptr;
};

} // namespace srsenb

#endif // SRSENB_METRICS_STDOUT_H
