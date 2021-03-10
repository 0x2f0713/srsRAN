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

/******************************************************************************
 * File:        enb.h
 * Description: Top-level eNodeB class. Creates and links all
 *              layers and helpers.
 *****************************************************************************/

#ifndef SRSENB_ENB_H
#define SRSENB_ENB_H

#include <pthread.h>
#include <stdarg.h>
#include <string>

#include "phy/phy.h"

#include "srslte/radio/radio.h"

#include "srsenb/hdr/phy/enb_phy_base.h"
#include "srsenb/hdr/stack/enb_stack_base.h"
#include "srsenb/hdr/stack/rrc/rrc_config.h"

#include "srslte/system/sys_metrics_processor.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/security.h"
#include "srslte/interfaces/enb_command_interface.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/sched_interface.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/srslog/srslog.h"

namespace srsenb {

/*******************************************************************************
  eNodeB Parameters
*******************************************************************************/

struct enb_args_t {
  uint32_t enb_id;
  uint32_t dl_earfcn; // By default the EARFCN from rr.conf's cell list are used but this value can be used for single
                      // cell eNB
  uint32_t n_prb;
  uint32_t nof_ports;
  uint32_t transmission_mode;
  float    p_a;
};

struct enb_files_t {
  std::string sib_config;
  std::string rr_config;
  std::string drb_config;
};

struct log_args_t {
  std::string all_level;
  int         phy_hex_limit;

  int         all_hex_limit;
  int         file_max_size;
  std::string filename;
};

struct gui_args_t {
  bool enable;
};

struct general_args_t {
  uint32_t    rrc_inactivity_timer;
  float       metrics_period_secs;
  bool        metrics_csv_enable;
  std::string metrics_csv_filename;
  bool        report_json_enable;
  std::string report_json_filename;
  bool        alarms_log_enable;
  std::string alarms_filename;
  bool        print_buffer_state;
  bool        tracing_enable;
  std::string tracing_filename;
  std::string eia_pref_list;
  std::string eea_pref_list;
};

struct all_args_t {
  enb_args_t        enb;
  enb_files_t       enb_files;
  srslte::rf_args_t rf;
  log_args_t        log;
  gui_args_t        gui;
  general_args_t    general;
  phy_args_t        phy;
  stack_args_t      stack;
};

struct rrc_cfg_t;

/*******************************************************************************
  Main eNB class
*******************************************************************************/

class enb : public enb_metrics_interface, enb_command_interface
{
public:
  enb(srslog::sink& log_sink);

  virtual ~enb();

  int init(const all_args_t& args_, srslte::logger* logger_);

  void stop();

  void start_plot();

  void print_pool();

  // eNodeB metrics interface
  bool get_metrics(enb_metrics_t* m) override;

  // eNodeB command interface
  void cmd_cell_gain(uint32_t cell_id, float gain) override;

private:
  const static int ENB_POOL_SIZE = 1024 * 10;

  int parse_args(const all_args_t& args_, rrc_cfg_t& rrc_cfg);

  srslte::logger*       logger = nullptr;
  srslog::sink&         log_sink;
  srslog::basic_logger& enb_log;

  all_args_t args    = {};
  bool       started = false;

  phy_cfg_t phy_cfg = {};
  rrc_cfg_t rrc_cfg = {};

  // eNB components
  std::unique_ptr<enb_stack_base>     stack = nullptr;
  std::unique_ptr<srslte::radio_base> radio = nullptr;
  std::unique_ptr<enb_phy_base>       phy   = nullptr;

  // System metrics processor.
  srslte::sys_metrics_processor sys_proc;

  srslte::LOG_LEVEL_ENUM level(std::string l);

  std::string get_build_mode();
  std::string get_build_info();
  std::string get_build_string();
};

} // namespace srsenb

#endif // SRSENB_ENB_H
