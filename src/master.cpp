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
#include "master_p.h"
#include "config.h"
//#include <chrono>
//#include <thread>
//#include <stdexcept>

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                             Master Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Master::Master (Master::Private &dd) : Device (dd) {

  }

  // ---------------------------------------------------------------------------
  Master::Master (Net net, const std::string & connection,
                  const std::string & settings) :
    Device (*new Private (this, net, connection, settings)) {}

  // ---------------------------------------------------------------------------
  Master::~Master() = default;

  // ---------------------------------------------------------------------------
  int Master::readDiscreteInputs (int addr, bool * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_read_input_bits (d->ctx,
                                     d->address (addr), nb, (uint8_t *) dest);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readCoils (int addr, bool * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_read_bits (d->ctx,
                               d->address (addr), nb, (uint8_t *) dest);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readInputRegisters (int addr, uint16_t * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_read_input_registers (d->ctx,
                                          d->address (addr), nb, dest);
    }

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readRegisters (int addr, uint16_t * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_read_registers (d->ctx,
                                    d->address (addr), nb, dest);
    }

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeCoil (int addr, bool value) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_bit (d->ctx,
                               d->address (addr), (uint8_t) value);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeCoils (int addr, const bool * src, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_bits (d->ctx,
                                d->address (addr), nb, (const uint8_t *) src);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeRegister (int addr, uint16_t value) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_register (d->ctx,
                                    d->address (addr), value);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeRegisters (int addr, const uint16_t * src, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_registers (d->ctx, d->address (addr), nb, src);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeReadRegisters (int waddr, const uint16_t * src, int wnb,
                                  int raddr, uint16_t * dest, int rnb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_and_read_registers (d->ctx,
                                              d->address (waddr), wnb, src,
                                              d->address (raddr), rnb, dest);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::reportSlaveId (int max_dest, uint8_t *dest) {
    
    if (isOpen() && net() == Rtu) {
      PIMP_D (Master);

      return modbus_report_slave_id (d->ctx, max_dest, dest);
    }
    return -1;

  }

  // ---------------------------------------------------------------------------
  //
  //                         Master::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Master::Private::Private (Master * q, Net net, const std::string & connection,
                            const std::string & settings) :
    Device::Private (q, net, connection, settings) {}

  // ---------------------------------------------------------------------------
  Master::Private::~Private() = default;
}

/* ========================================================================== */
