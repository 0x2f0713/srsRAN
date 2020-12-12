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

#ifndef SRSLTE_RWLOCK_GUARD_H
#define SRSLTE_RWLOCK_GUARD_H

#include <pthread.h>

namespace srslte {

class rwlock_write_guard
{
public:
  rwlock_write_guard(pthread_rwlock_t& rwlock_) : rwlock(&rwlock_) { pthread_rwlock_wrlock(rwlock); }
  rwlock_write_guard(const rwlock_write_guard&) = delete;
  rwlock_write_guard(rwlock_write_guard&&)      = delete;
  rwlock_write_guard& operator=(const rwlock_write_guard&) = delete;
  rwlock_write_guard& operator=(rwlock_write_guard&&) = delete;
  ~rwlock_write_guard() { pthread_rwlock_unlock(rwlock); }

private:
  pthread_rwlock_t* rwlock;
};

// Shared lock guard that automatically unlocks rwlock on exit
class rwlock_read_guard
{
public:
  rwlock_read_guard(pthread_rwlock_t& rwlock_) : rwlock(&rwlock_) { pthread_rwlock_rdlock(rwlock); }
  rwlock_read_guard(const rwlock_read_guard&) = delete;
  rwlock_read_guard(rwlock_read_guard&&)      = delete;
  rwlock_read_guard& operator=(const rwlock_read_guard&) = delete;
  rwlock_read_guard& operator=(rwlock_read_guard&&) = delete;
  ~rwlock_read_guard() { pthread_rwlock_unlock(rwlock); }

private:
  pthread_rwlock_t* rwlock;
};

} // namespace srslte

#endif // SRSLTE_RWLOCK_GUARD_H
