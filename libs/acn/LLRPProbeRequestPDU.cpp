/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * LLRPProbeRequestPDU.cpp
 * The LLRPProbeRequestPDU
 * Copyright (C) 2020 Peter Newman
 */

#include "libs/acn/LLRPProbeRequestPDU.h"

#include <ola/network/NetworkUtils.h>
#include <ola/rdm/UID.h>
#include <ola/rdm/UIDSet.h>

namespace ola {
namespace acn {

using ola::io::OutputStream;
using ola::network::HostToNetwork;
using ola::rdm::UID;
using std::vector;

void LLRPProbeRequestPDU::PrependPDU(ola::io::IOStack *stack,
                                     const UID &lower_uid,
                                     const UID &upper_uid,
                                     bool client_tcp_connection_inactive,
                                     bool brokers_only,
                                     const ola::rdm::UIDSet &known_uids) {
  llrp_probe_request_pdu_data data;
  lower_uid.Pack(data.lower_uid, sizeof(data.lower_uid));
  upper_uid.Pack(data.upper_uid, sizeof(data.upper_uid));
  uint16_t filter = 0;
  if (client_tcp_connection_inactive) {
    filter |= FILTER_CLIENT_TCP_CONNECTION_INACTIVE;
  }
  if (brokers_only) {
    filter |= FILTER_BROKERS_ONLY;
  }
  data.filter = HostToNetwork(filter);
  known_uids.Pack(data.known_uids, sizeof(data.known_uids));
  stack->Write(reinterpret_cast<uint8_t*>(&data),
               static_cast<unsigned int>(sizeof(llrp_probe_request_pdu_data) -
                                         sizeof(data.known_uids) +
                                         (known_uids.Size() * UID::LENGTH)));
  uint8_t vector = HostToNetwork(VECTOR_PROBE_REQUEST_DATA);
  stack->Write(reinterpret_cast<uint8_t*>(&vector), sizeof(vector));
  PrependFlagsAndLength(stack);
}
}  // namespace acn
}  // namespace ola
