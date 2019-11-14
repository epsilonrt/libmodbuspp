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
#include "slave_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         Slave Class
  //
  // ---------------------------------------------------------------------------
  
  // ---------------------------------------------------------------------------
  int Slave::readDiscreteInputs (int addr, bool * dest, int nb) {
    PIMP_D (Slave);

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::readCoils (int addr, bool * dest, int nb) {
    PIMP_D (Slave);

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::readInputRegisters (int addr, uint16_t * dest, int nb) {
    PIMP_D (Slave);

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::readRegisters (int addr, uint16_t * dest, int nb) {
    PIMP_D (Slave);

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::writeCoil (int addr, bool value) {
    PIMP_D (Slave);

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::writeCoils (int addr, const bool * src, int nb) {
    PIMP_D (Slave);

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::writeRegister (int addr, uint16_t value) {
    PIMP_D (Slave);

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::writeRegisters (int addr, const uint16_t * src, int nb) {
    PIMP_D (Slave);

    return -1;
  }
}

/* ========================================================================== */
