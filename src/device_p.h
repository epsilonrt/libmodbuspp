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

#include <modbuspp/device.h>
#include "rtulayer_p.h"
#include "tcplayer_p.h"

namespace Modbus {

  class Device::Private {

    public:
      Private (Device * q, Net net, const std::string & connection, const std::string & settings);
      virtual ~Private();
      int address (int addr);

      Device * const q_ptr;
      modbus_t * ctx;
      bool isOpen;
      bool pduAddressing;
      RtuLayer * rtu;
      TcpLayer * tcp;
      int slave;

      friend class RtuLayer::Private;
      friend class TcpLayer::Private;

      PIMP_DECLARE_PUBLIC (Device)
  };
}

/* ========================================================================== */
