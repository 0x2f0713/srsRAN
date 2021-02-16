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

#include "srslte/common/byte_buffer.h"
#include "srslte/interfaces/pdcp_interface_types.h"
#include <map>

#ifndef SRSLTE_ENB_PDCP_INTERFACES_H
#define SRSLTE_ENB_PDCP_INTERFACES_H

namespace srsenb {

// PDCP interface for GTPU
class pdcp_interface_gtpu
{
public:
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu, int pdcp_sn = -1) = 0;
  virtual std::map<uint32_t, srslte::unique_byte_buffer_t> get_buffered_pdus(uint16_t rnti, uint32_t lcid) = 0;
};

// PDCP interface for RRC
class pdcp_interface_rrc
{
public:
  virtual void reset(uint16_t rnti)                                                                        = 0;
  virtual void add_user(uint16_t rnti)                                                                     = 0;
  virtual void rem_user(uint16_t rnti)                                                                     = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu, int pdcp_sn = -1) = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, srslte::pdcp_config_t cnfg)                        = 0;
  virtual void del_bearer(uint16_t rnti, uint32_t lcid)                                                    = 0;
  virtual void config_security(uint16_t rnti, uint32_t lcid, srslte::as_security_config_t sec_cfg)         = 0;
  virtual void enable_integrity(uint16_t rnti, uint32_t lcid)                                              = 0;
  virtual void enable_encryption(uint16_t rnti, uint32_t lcid)                                             = 0;
  virtual void send_status_report(uint16_t rnti, uint32_t lcid)                                            = 0;
  virtual bool get_bearer_state(uint16_t rnti, uint32_t lcid, srslte::pdcp_lte_state_t* state)             = 0;
  virtual bool set_bearer_state(uint16_t rnti, uint32_t lcid, const srslte::pdcp_lte_state_t& state)       = 0;
  virtual void reestablish(uint16_t rnti)                                                                  = 0;
};

// PDCP interface for RLC
class pdcp_interface_rlc
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)            = 0;
  virtual void notify_delivery(uint16_t rnti, uint32_t lcid, const std::vector<uint32_t>& pdcp_sns) = 0;
};

} // namespace srsenb

#endif // SRSLTE_ENB_PDCP_INTERFACES_H
