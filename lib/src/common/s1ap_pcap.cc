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

#include "srsran/common/s1ap_pcap.h"
#include "srsran/common/pcap.h"
#include "srsran/srsran.h"
#include <stdint.h>

namespace srsran {

void s1ap_pcap::enable()
{
  enable_write = true;
}
void s1ap_pcap::open(const char* filename)
{
  pcap_file    = DLT_PCAP_Open(S1AP_LTE_DLT, filename);
  enable_write = true;
}
void s1ap_pcap::close()
{
  fprintf(stdout, "Saving S1AP PCAP file\n");
  DLT_PCAP_Close(pcap_file);
}

void s1ap_pcap::write_s1ap(uint8_t* pdu, uint32_t pdu_len_bytes)
{
  if (enable_write) {
    S1AP_Context_Info_t context;
    if (pdu) {
      LTE_PCAP_S1AP_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
    }
  }
}

} // namespace srsran
