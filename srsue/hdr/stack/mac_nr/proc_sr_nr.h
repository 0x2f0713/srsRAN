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

#ifndef SRSUE_PROC_SR_NR_H
#define SRSUE_PROC_SR_NR_H

#include "srsran/interfaces/ue_mac_interfaces.h"
#include "srsran/srslog/srslog.h"
#include <stdint.h>

/// Scheduling Request procedure as defined in 5.4.4 of 38.321
/// Note: currently only a single SR config for all logical channels is supported

namespace srsue {

class proc_ra_nr;
class phy_interface_mac_nr;
class rrc_interface_mac;

class proc_sr_nr
{
public:
  explicit proc_sr_nr(srslog::basic_logger& logger);
  int32_t init(proc_ra_nr* ra_, phy_interface_mac_nr* phy_, rrc_interface_mac* rrc_);
  void    step(uint32_t tti);
  int32_t set_config(const srsran::sr_cfg_nr_t& cfg);
  void    reset();
  void    start();
  bool    sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx);

private:
  int  sr_counter    = 0;
  bool is_pending_sr = 0;

  srsran::sr_cfg_nr_t cfg = {};

  proc_ra_nr*           ra  = nullptr;
  rrc_interface_mac*    rrc = nullptr;
  phy_interface_mac_nr* phy = nullptr;
  srslog::basic_logger& logger;

  bool initiated = false;
};

} // namespace srsue

#endif // SRSUE_PROC_SR_H
