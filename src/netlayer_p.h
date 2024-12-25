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

#include <modbuspp/netlayer.h>

namespace Modbus {

  class NetLayer::Private {
    public:
      Private (Net n, const std::string & c, const std::string & s, uint16_t m) :
        net (n), connection (c), settings (s), maxAduLength (m)  {}

      Net net;
      std::string connection;
      std::string settings;
      uint16_t maxAduLength;
      std::unique_ptr<modbus_t, void(*)(modbus_t*)> ctx =
        {nullptr, [](modbus_t* p){ modbus_free(p); }};
  };
}

/* ========================================================================== */
