/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
 * This file is part of the libmodbuspp Library.
 *
 * The libmodbuspp Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libmodbuspp Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libmodbuspp Library; if not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <modbuspp/message.h>

namespace Modbus {

  class Message::Private {
    public:
      Private (Message * q, NetLayer * b);
      Private (Message * q, NetLayer * b, const std::vector<uint8_t> & m);
      Private (Message * q, NetLayer * b, Function f);
      Private (Message * q, Net n);
      Private (Message * q, Net n, const std::vector<uint8_t> & m);
      Private (Message * q, Net n, Function f);
      virtual ~Private() = default;

      Message * const q_ptr = nullptr;
      Net net;
      int pduBegin;
      size_t aduSize;
      uint16_t maxAduLength;
      bool isResponse;
      NetLayer * backend = nullptr;
      uint16_t transactionId;
      std::vector<uint8_t> adu;
  };
}

/* ========================================================================== */
