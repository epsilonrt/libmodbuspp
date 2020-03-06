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

#include <vector>
#include <modbuspp/bufferedslave.h>
#include "slave_p.h"

namespace Modbus {

  class BufferedSlave::Private : public Slave::Private {

    public:
      Private (BufferedSlave * q);
      Private (BufferedSlave * q, int s, Device * d);
      virtual ~Private();
      
      int setDiscreteInputBlock (int addr, int nmemb);
      int setCoilBlock (int addr, int nmemb);
      int setHoldingRegisterBlock (int addr, int nmemb);
      int setInputRegisterBlock (int addr, int nmemb);
      int updateDiscreteInputBlockFromSlave();
      int updateCoilBlockFromSlave();
      int updateHoldingRegisterBlockFromSlave();
      int updateInputRegisterBlockFromSlave();
      int updateSlaveCoilFromBlock();
      int updateSlaveHoldingRegisterFromBlock();

      modbus_mapping_t * map;
      std::vector<uint8_t> idReport;
      Message::Callback beforeReplyCB;
      Message::Callback afterReplyCB;

      PIMP_DECLARE_PUBLIC (BufferedSlave)
  };
}

/* ========================================================================== */
