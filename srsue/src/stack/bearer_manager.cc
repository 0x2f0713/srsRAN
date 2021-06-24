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

#include "srsue/hdr/stack/bearer_manager.h"

namespace srsue {

bearer_manager::bearer_manager() : logger(srslog::fetch_basic_logger("STCK", false))
{
  pthread_rwlock_init(&rwlock, nullptr);
}

bearer_manager::~bearer_manager()
{
  pthread_rwlock_destroy(&rwlock);
}

void bearer_manager::add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid)
{
  srsran::rwlock_write_guard rw_lock(rwlock);
  auto                       it = eps_rb_map.find(eps_bearer_id);
  if (it != eps_rb_map.end()) {
    logger.error("EPS bearer ID %d already registered", eps_bearer_id);
    return;
  }
  eps_rb_map.emplace(eps_bearer_id, radio_bearer_t{rat, lcid});
  logger.info("Registered EPS bearer ID %d for lcid=%d over %s-PDCP", eps_bearer_id, lcid, to_string(rat).c_str());
}

/// EPS bearer is removed from map when the associated DRB is deleted (e.g. after connection release)
void bearer_manager::remove_eps_bearer(uint8_t eps_bearer_id)
{
  srsran::rwlock_write_guard rw_lock(rwlock);
  auto                       it = eps_rb_map.find(eps_bearer_id);
  if (it == eps_rb_map.end()) {
    logger.error("Can't remove EPS bearer ID %d", eps_bearer_id);
    return;
  }
  eps_rb_map.erase(it);
  logger.info("Removed mapping for EPS bearer ID %d", eps_bearer_id);
}

// GW interface
bool bearer_manager::has_active_radio_bearer(uint32_t eps_bearer_id)
{
  srsran::rwlock_read_guard rw_lock(rwlock);
  return eps_rb_map.find(eps_bearer_id) != eps_rb_map.end();
}

// Stack interface
bearer_manager::radio_bearer_t& bearer_manager::get_radio_bearer(uint32_t eps_bearer_id)
{
  srsran::rwlock_read_guard rw_lock(rwlock);
  if (eps_rb_map.find(eps_bearer_id) != eps_rb_map.end()) {
    return eps_rb_map.at(eps_bearer_id);
  }
  return invalid_rb;
}

} // namespace srsue