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

#include <map>
#include <modbuspp/master.h>
#include "device_p.h"

namespace Modbus {

  class Master::Private : public Device::Private {

    public:
      Private (Master * q);

      void setBackend (Net net, const std::string & connection,
                               const std::string & settings) override;
      void setConfig (const nlohmann::json & config) override;

      Slave * addSlave (int slaveAddr);

      std::map <int, std::shared_ptr<Slave>> slave;
      PIMP_DECLARE_PUBLIC (Master)
  };
}

/* ========================================================================== */

