/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSENB_RRC_MOBILITY_H
#define SRSENB_RRC_MOBILITY_H

#include "rrc.h"
#include <map>

namespace srsenb {

bool operator==(const asn1::rrc::report_cfg_eutra_s& lhs, const asn1::rrc::report_cfg_eutra_s& rhs);

/**
 * This class is responsible for storing the UE Measurement Configuration at the eNB side.
 * Has the same fields as asn1::rrc::var_meas_cfg but stored in data structs that are easier to handle
 */
class var_meas_cfg_t
{
public:
  explicit var_meas_cfg_t(srslte::log* log_) : rrc_log(log_) {}
  using meas_cell_t  = asn1::rrc::cells_to_add_mod_s;
  using meas_id_t    = asn1::rrc::meas_id_to_add_mod_s;
  using meas_obj_t   = asn1::rrc::meas_obj_to_add_mod_s;
  using report_cfg_t = asn1::rrc::report_cfg_to_add_mod_s;

  std::tuple<bool, meas_obj_t*, meas_cell_t*> add_cell_cfg(const meas_cell_cfg_t& cellcfg);
  report_cfg_t*                               add_report_cfg(const asn1::rrc::report_cfg_eutra_s& reportcfg);
  meas_id_t*                                  add_measid_cfg(uint8_t measobjid, uint8_t repid);

  bool compute_diff_meas_cfg(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_meas_objs(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_cells(const asn1::rrc::meas_obj_eutra_s& target_it,
                          const asn1::rrc::meas_obj_eutra_s& src_it,
                          asn1::rrc::meas_obj_to_add_mod_s*  added_obj) const;
  void compute_diff_report_cfgs(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_meas_ids(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;

  // getters
  const asn1::rrc::meas_obj_to_add_mod_list_l&   meas_objs() const { return var_meas.meas_obj_list; }
  const asn1::rrc::report_cfg_to_add_mod_list_l& rep_cfgs() const { return var_meas.report_cfg_list; }

private:
  asn1::rrc::var_meas_cfg_s var_meas;
  srslte::log*              rrc_log = nullptr;
};

class rrc::mobility_cfg
{
public:
  explicit mobility_cfg(rrc* outer_rrc);

  std::shared_ptr<const var_meas_cfg_t> current_meas_cfg; ///< const to enable ptr comparison as identity comparison

private:
  rrc* rrc_enb = nullptr;
};

class rrc::ue::rrc_mobility
{
public:
  explicit rrc_mobility(srsenb::rrc::ue* outer_ue);
  bool fill_conn_recfg_msg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg);

private:
  rrc::ue*                  rrc_ue  = nullptr;
  rrc*                      rrc_enb = nullptr;
  rrc::mobility_cfg*        cfg     = nullptr;
  srslte::byte_buffer_pool* pool    = nullptr;
  srslte::log*              rrc_log = nullptr;

  // vars
  std::shared_ptr<const var_meas_cfg_t> ue_var_meas;

  class mobility_proc_t
  {
  public:
    srslte::proc_outcome_t init() { return srslte::proc_outcome_t::yield; }
    srslte::proc_outcome_t step() { return srslte::proc_outcome_t::yield; }

  private:
    enum class state_t { ho_started };
  };
  srslte::proc_t<mobility_proc_t> mobility_proc;
};

} // namespace srsenb
#endif // SRSENB_RRC_MOBILITY_H
