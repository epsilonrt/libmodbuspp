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

#include <fstream>
#include <exception>
#include <modbuspp/device.h>
#include <modbuspp/netlayer.h>
#include "json_p.h"

namespace Modbus {

  class Device::Private {

    public:
      Private (Device * q);
      virtual ~Private();
      virtual void setBackend (Net net, const std::string & connection,
                               const std::string & settings);
      virtual void setConfig (const nlohmann::json & config);

      void setConfigFromFile (const std::string & jsonfile,
                              const std::string & key);

      virtual bool open();
      virtual void close();
      inline modbus_t * ctx() {
        return backend->context();
      }
      inline modbus_t * ctx() const {
        return backend->context();
      }
      int defaultSlave (int addr) const;
      bool isConnected () const;

      Device * const q_ptr;
      bool isOpen;
      NetLayer * backend;
      bool recoveryLink;
      bool debug;

      PIMP_DECLARE_PUBLIC (Device)
  };
}

/* ========================================================================== */
