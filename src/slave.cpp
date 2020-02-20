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
  Slave::Slave (Slave::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Slave::Slave () : d_ptr (new Private (this)) {}

  // ---------------------------------------------------------------------------
  Slave::Slave (int slaveAddr, Device * dev) :
    d_ptr (new Private (this, slaveAddr, dev)) {}

  // ---------------------------------------------------------------------------
  Slave::~Slave() = default;

  // ---------------------------------------------------------------------------
  int Slave::number() const {
    PIMP_D (const Slave);

    return d->id;
  }

  // ---------------------------------------------------------------------------
  void Slave::setNumber (int id) {
    PIMP_D (Slave);

    d->id = id;
  }

  // ---------------------------------------------------------------------------
  Device * Slave::device() const {
    PIMP_D (const Slave);

    return d->dev;
  }

  // ---------------------------------------------------------------------------
  void Slave::setDevice (Device * dev) {
    PIMP_D (Slave);

    d->dev = dev;
  }

  // ---------------------------------------------------------------------------
  bool Slave::isValid() const {

    return (device() != 0) && (number() >= 0);
  }

  // ---------------------------------------------------------------------------
  bool Slave::isOpen() const {
    PIMP_D (const Slave);

    return d->dev ? d->dev->isOpen() : false;
  }

  // ---------------------------------------------------------------------------
  int Slave::readCoils (int addr, bool * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Slave);

      return modbus_read_bits (d->ctx(),
                               pduAddress (addr), nb, (uint8_t *) dest);
    }
    return -1;

  }

  // ---------------------------------------------------------------------------
  int Slave::readDiscreteInputs (int addr, bool * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Slave);

      return modbus_read_input_bits (d->ctx(),
                                     pduAddress (addr), nb, (uint8_t *) dest);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::readRegisters (int addr, uint16_t * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Slave);

      if (modbus_set_slave (d->ctx(), d->id) == 0) {

        return modbus_read_registers (d->ctx(),
                                      pduAddress (addr), nb, dest);
      }
    }

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::readInputRegisters (int addr, uint16_t * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Slave);

      if (modbus_set_slave (d->ctx(), d->id) == 0) {

        return modbus_read_input_registers (d->ctx(),
                                            pduAddress (addr), nb, dest);
      }
    }

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::writeCoils (int addr, const bool * src, int nb) {

    if (isOpen()) {
      PIMP_D (Slave);

      if (modbus_set_slave (d->ctx(), d->id) == 0) {

        return modbus_write_bits (d->ctx(),
                                  pduAddress (addr), nb, (const uint8_t *) src);
      }
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::writeCoil (int addr, bool value) {

    if (isOpen()) {
      PIMP_D (Slave);

      if (modbus_set_slave (d->ctx(), d->id) == 0) {

        return modbus_write_bit (d->ctx(),
                                 pduAddress (addr), (uint8_t) value);
      }
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::writeRegisters (int addr, const uint16_t * src, int nb) {

    if (isOpen()) {
      PIMP_D (Slave);

      if (modbus_set_slave (d->ctx(), d->id) == 0) {

        return modbus_write_registers (d->ctx(), pduAddress (addr), nb, src);
      }
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::writeRegister (int addr, uint16_t value) {

    if (isOpen()) {
      PIMP_D (Slave);

      if (modbus_set_slave (d->ctx(), d->id) == 0) {

        return modbus_write_register (d->ctx(),
                                      pduAddress (addr), value);
      }
    }
    return -1;
  }


  // ---------------------------------------------------------------------------
  int Slave::writeReadRegisters (int waddr, const uint16_t * src, int wnb,
                                 int raddr, uint16_t * dest, int rnb) {

    if (isOpen()) {
      PIMP_D (Slave);

      if (modbus_set_slave (d->ctx(), d->id) == 0) {

        return modbus_write_and_read_registers (d->ctx(),
                                                pduAddress (waddr), wnb, src,
                                                pduAddress (raddr), rnb, dest);
      }
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Slave::reportSlaveId (uint16_t max_dest, uint8_t *dest) {

    if (isOpen()) {
      PIMP_D (Slave);

      if (d->dev->net() == Rtu) {
        if (modbus_set_slave (d->ctx(), d->id) == 0) {

          return modbus_report_slave_id (d->ctx(), max_dest, dest);
        }
      }
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  void Slave::setPduAddressing (bool pdu) {
    PIMP_D (Slave);

    d->pduAddressing = pdu;
  }

  // ---------------------------------------------------------------------------
  bool  Slave::pduAddressing() const {
    PIMP_D (const Slave);

    return d->pduAddressing;
  }

  // ---------------------------------------------------------------------------
  int Slave::pduAddress (int dataAddr) const {
    PIMP_D (const Slave);

    return dataAddr - (d->pduAddressing ? 0 : 1);
  }

  // ---------------------------------------------------------------------------
  // static
  void Slave::setBoolArray (bool * dest, const uint8_t * src, size_t n) {

    modbus_set_bits_from_bytes ( (uint8_t *) dest, 0, n, src);
  }

  // ---------------------------------------------------------------------------
  //
  //                         Slave::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Slave::Private::Private (Slave * q) :
    q_ptr (q), pduAddressing (false), id (-1), dev (0) {}

  // ---------------------------------------------------------------------------
  Slave::Private::Private (Slave * q, int s, Device * d) :
    Private (q) {

    id = s;
    dev = d;
  }

  // ---------------------------------------------------------------------------
  Slave::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  //
  //                         Modbus::Json Namespace
  //
  // ---------------------------------------------------------------------------
  namespace Json {

    // -------------------------------------------------------------------------
    void setConfig (Slave * s, const nlohmann::json & j) {

      auto i = j["id"].get<int>();
      s->setNumber (i);

      if (j.contains ("pdu-adressing")) {

        auto b = j["pdu-adressing"].get<bool>();
        s->setPduAddressing (b);
      }
    }
  }
}

/* ========================================================================== */
