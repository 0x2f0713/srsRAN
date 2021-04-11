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

#include "srsran/upper/bearer_mem_pool.h"
#include "srsran/adt/pool/batch_mem_pool.h"
#include "srsran/upper/rlc_am_lte.h"
#include "srsran/upper/rlc_um_lte.h"
#include "srsran/upper/rlc_um_nr.h"

namespace srsran {

srsran::background_mem_pool* get_bearer_pool()
{
  static background_mem_pool pool(
      4, std::max(std::max(sizeof(rlc_am_lte), sizeof(rlc_um_lte)), sizeof(rlc_um_nr)), 8, 8);
  return &pool;
}

void reserve_rlc_memblocks(size_t nof_blocks)
{
  srsran::background_mem_pool* pool = get_bearer_pool();
  while (pool->cache_size() < nof_blocks) {
    pool->allocate_batch();
  }
}
void* allocate_rlc_bearer(std::size_t sz)
{
  return get_bearer_pool()->allocate_node(sz);
}
void deallocate_rlc_bearer(void* p)
{
  get_bearer_pool()->deallocate_node(p);
}

} // namespace srsran
