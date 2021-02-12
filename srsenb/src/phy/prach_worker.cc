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

#include "srsenb/hdr/phy/prach_worker.h"
#include "srslte/interfaces/enb_mac_interfaces.h"
#include "srslte/srslte.h"

namespace srsenb {

int prach_worker::init(const srslte_cell_t&      cell_,
                       const srslte_prach_cfg_t& prach_cfg_,
                       stack_interface_phy_lte*  stack_,
                       int                       priority,
                       uint32_t                  nof_workers_)
{
  stack       = stack_;
  prach_cfg   = prach_cfg_;
  cell        = cell_;
  nof_workers = nof_workers_;

  max_prach_offset_us = 50;

  if (srslte_prach_init(&prach, srslte_symbol_sz(cell.nof_prb))) {
    return -1;
  }

  if (srslte_prach_set_cfg(&prach, &prach_cfg, cell.nof_prb)) {
    ERROR("Error initiating PRACH");
    return -1;
  }

  srslte_prach_set_detect_factor(&prach, 60);

  nof_sf = (uint32_t)ceilf(prach.T_tot * 1000);

  if (nof_workers > 0) {
    start(priority);
  }

  initiated = true;

  sf_cnt = 0;

#if defined(ENABLE_GUI) and ENABLE_PRACH_GUI
  char title[32] = {};
  snprintf(title, sizeof(title), "PRACH buffer %d", cc_idx);

  sdrgui_init();
  plot_real_init(&plot_real);
  plot_real_setTitle(&plot_real, title);
  plot_real_setXAxisAutoScale(&plot_real, true);
  plot_real_setYAxisAutoScale(&plot_real, true);
  plot_real_addToWindowGrid(&plot_real, (char*)"PRACH", 0, cc_idx);
#endif // defined(ENABLE_GUI) and ENABLE_PRACH_GUI

  return 0;
}

void prach_worker::stop()
{
  running      = false;
  sf_buffer* s = nullptr;
  pending_buffers.push(s);

  if (nof_workers > 0) {
    wait_thread_finish();
  }

  srslte_prach_free(&prach);
}

void prach_worker::set_max_prach_offset_us(float delay_us)
{
  max_prach_offset_us = delay_us;
}

int prach_worker::new_tti(uint32_t tti_rx, cf_t* buffer_rx)
{
  // Save buffer only if it's a PRACH TTI
  if (srslte_prach_tti_opportunity(&prach, tti_rx, -1) || sf_cnt) {
    if (sf_cnt == 0) {
      current_buffer = buffer_pool.allocate();
      if (!current_buffer) {
        logger.warning("PRACH skipping tti=%d due to lack of available buffers", tti_rx);
        return 0;
      }
    }
    if (!current_buffer) {
      logger.error("PRACH: Expected available current_buffer");
      return -1;
    }
    if (current_buffer->nof_samples + SRSLTE_SF_LEN_PRB(cell.nof_prb) < sf_buffer_sz) {
      memcpy(&current_buffer->samples[sf_cnt * SRSLTE_SF_LEN_PRB(cell.nof_prb)],
             buffer_rx,
             sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
      current_buffer->nof_samples += SRSLTE_SF_LEN_PRB(cell.nof_prb);
      if (sf_cnt == 0) {
        current_buffer->tti = tti_rx;
      }
    } else {
      logger.error("PRACH: Not enough space in current_buffer");
      return -1;
    }
    sf_cnt++;
    if (sf_cnt == nof_sf) {
      sf_cnt = 0;
      if (nof_workers == 0) {
        run_tti(current_buffer);
        current_buffer->reset();
        buffer_pool.deallocate(current_buffer);
      } else {
        pending_buffers.push(current_buffer);
      }
    }
  }
  return 0;
}

int prach_worker::run_tti(sf_buffer* b)
{
  uint32_t prach_nof_det = 0;
  if (srslte_prach_tti_opportunity(&prach, b->tti, -1)) {
    // Detect possible PRACHs
    if (srslte_prach_detect_offset(&prach,
                                   prach_cfg.freq_offset,
                                   &b->samples[prach.N_cp],
                                   nof_sf * SRSLTE_SF_LEN_PRB(cell.nof_prb) - prach.N_cp,
                                   prach_indices,
                                   prach_offsets,
                                   prach_p2avg,
                                   &prach_nof_det)) {
      logger.error("Error detecting PRACH");
      return SRSLTE_ERROR;
    }

    if (prach_nof_det) {
      for (uint32_t i = 0; i < prach_nof_det; i++) {
        logger.info("PRACH: cc=%d, %d/%d, preamble=%d, offset=%.1f us, peak2avg=%.1f, max_offset=%.1f us",
                    cc_idx,
                    i,
                    prach_nof_det,
                    prach_indices[i],
                    prach_offsets[i] * 1e6,
                    prach_p2avg[i],
                    max_prach_offset_us);

        if (prach_offsets[i] * 1e6 < max_prach_offset_us) {
          // Convert time offset to Time Alignment command
          uint32_t n_ta = (uint32_t)(prach_offsets[i] / (16 * SRSLTE_LTE_TS));

          stack->rach_detected(b->tti, cc_idx, prach_indices[i], n_ta);

#if defined(ENABLE_GUI) and ENABLE_PRACH_GUI
          uint32_t nof_samples = SRSLTE_MIN(nof_sf * SRSLTE_SF_LEN_PRB(cell.nof_prb), 3 * SRSLTE_SF_LEN_MAX);
          srslte_vec_abs_cf(b->samples, plot_buffer.data(), nof_samples);
          plot_real_setNewData(&plot_real, plot_buffer.data(), nof_samples);
#endif // defined(ENABLE_GUI) and ENABLE_PRACH_GUI
        }
      }
    }
  }
  return 0;
}

void prach_worker::run_thread()
{
  running = true;
  while (running) {
    sf_buffer* b = pending_buffers.wait_pop();
    if (running && b) {
      int ret = run_tti(b);
      b->reset();
      buffer_pool.deallocate(b);
      if (ret) {
        running = false;
      }
    }
  }
}

} // namespace srsenb
