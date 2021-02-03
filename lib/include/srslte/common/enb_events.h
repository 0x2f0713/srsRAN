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

#ifndef SRSENB_ENB_EVENTS_H
#define SRSENB_ENB_EVENTS_H

#include <cstdint>
#include <memory>

namespace srslog {
class log_channel;
}

namespace srsenb {

/// This interface logs different kinds of events to the configured channel. By default, if no log channel is configured
/// logging will be disabled.
class event_logger_interface
{
public:
  virtual ~event_logger_interface() = default;

  /// Logs into the underlying log channel the RRC connected event.
  virtual void log_rrc_connected(unsigned cause) = 0;

  /// Logs into the underlying log channel the RRC disconnected event.
  virtual void log_rrc_disconnect(unsigned reason) = 0;

  /// Logs into the underlying log channel the S1 context create event.
  virtual void log_s1_ctx_create(uint32_t mme_id, uint32_t enb_id, uint16_t rnti) = 0;

  /// Logs into the underlying log channel the S1 context delete event.
  virtual void log_s1_ctx_delete(uint32_t mme_id, uint32_t enb_id, uint16_t rnti) = 0;

  /// Logs into the underlying log channel when a sector has been started.
  virtual void log_sector_start(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) = 0;

  /// Logs into the underlying log channel when a sector has been stopped.
  virtual void log_sector_stop(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) = 0;

  /// Logs into the underlying log channel a RLF event.
  virtual void log_rlf(uint32_t cc_idx, const std::string& asn1, uint16_t rnti) = 0;
};

/// Singleton class to provide global access to the event_logger_interface interface.
class event_logger
{
  event_logger() = default;

public:
  /// Returns the instance of the event logger.
  static event_logger_interface& get();

  /// Uses the specified log channel for event logging.
  /// NOTE: This method is not thread safe.
  static void configure(srslog::log_channel& c);

private:
  static std::unique_ptr<event_logger_interface> pimpl;
};

} // namespace srsenb

#endif // SRSENB_ENB_EVENTS_H
