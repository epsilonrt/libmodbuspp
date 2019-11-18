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
#include <modbuspp/netlayer.h>

namespace Modbus {
  
  class Device::Private {

    public:
      Private (Device * q, Net net, const std::string & connection,
               const std::string & settings);
      Private (Device * q, const Private & other);
      virtual ~Private();
      inline modbus_t * ctx() {
        return backend->context();
      }
      inline modbus_t * ctx() const {
        return backend->context();
      }
      int defaultSlave (int addr) const;

      Device * const q_ptr;
      bool isOpen;
      NetLayer * backend;
      bool recoveryLink;

      PIMP_DECLARE_PUBLIC (Device)
  };
}

/* ========================================================================== */
