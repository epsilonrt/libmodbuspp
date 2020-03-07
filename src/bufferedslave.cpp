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
#include "bufferedslave_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         BufferedSlave Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  BufferedSlave::BufferedSlave (BufferedSlave::Private &dd) : Slave (dd) {}

  // ---------------------------------------------------------------------------
  BufferedSlave::BufferedSlave () : Slave (*new Private (this)) {}

  // ---------------------------------------------------------------------------
  BufferedSlave::BufferedSlave (int s, Device * d) :
    Slave (*new Private (this, s, d)) {}

  // ---------------------------------------------------------------------------
  BufferedSlave::~BufferedSlave() = default;

  // ---------------------------------------------------------------------------
  bool BufferedSlave::isValid() const {

    return (number() >= 0);
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::setBlock (Table t, int nmemb, int startAddr) {
    PIMP_D (BufferedSlave);

    if (startAddr < 0) {

      startAddr = d->pduAddressing ? 0 : 1;
    }

    switch (t) {
      case DiscreteInput:
        return d->setDiscreteInputBlock (startAddr, nmemb);
      case Coil:
        return d->setCoilBlock (startAddr, nmemb);
      case InputRegister:
        return d->setInputRegisterBlock (startAddr, nmemb);
      case HoldingRegister:
        return d->setHoldingRegisterBlock (startAddr, nmemb);
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  void BufferedSlave::setBeforeReplyCallback (Message::Callback cb) {
    PIMP_D (BufferedSlave);
    d->beforeReplyCB = cb;
  }

  // ---------------------------------------------------------------------------
  Message::Callback BufferedSlave::beforeReplyCallback() const {
    PIMP_D (const BufferedSlave);
    return d->beforeReplyCB;
  }

  // ---------------------------------------------------------------------------
  Message::Callback BufferedSlave::afterReplyCallback() const {
    PIMP_D (const BufferedSlave);
    return d->afterReplyCB;
  }

  // ---------------------------------------------------------------------------
  void BufferedSlave::setAfterReplyCallback (Message::Callback cb) {
    PIMP_D (BufferedSlave);
    d->afterReplyCB = cb;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::readFromDevice (const Request * req) {

    if (req) {
      PIMP_D (BufferedSlave);

      if (isOpen()) {
        int offset;
        int start = req->startingAddress();
        int nb = req->quantity();

        switch (req->function()) {

          case ReadCoils: {
            start = std::max (start, d->map->start_bits);
            start = std::min (start, d->map->start_bits + d->map->nb_bits - 1);
            offset = start - d->map->start_bits;
            nb = std::min (nb, (d->map->nb_bits - offset));
            bool * dest = reinterpret_cast <bool *> (&d->map->tab_bits[offset]);

            return Slave::readCoils (start + pduAddressing() ? 0 : 1, dest, nb);
          }
          break;

          case ReadDiscreteInputs: {
            start = std::max (start, d->map->start_input_bits);
            start = std::min (start, d->map->start_input_bits + d->map->nb_input_bits - 1);
            offset = start - d->map->start_input_bits;
            nb = std::min (nb, (d->map->nb_input_bits - offset));
            bool * dest = reinterpret_cast <bool *> (&d->map->tab_input_bits[offset]);

            return Slave::readDiscreteInputs (start + pduAddressing() ? 0 : 1, dest, nb);
          }
          break;

          case ReadHoldingRegisters: {
            start = std::max (start, d->map->start_registers);
            start = std::min (start, d->map->start_registers + d->map->nb_registers - 1);
            offset = start - d->map->start_registers;
            nb = std::min (nb, (d->map->nb_registers - offset));
            uint16_t * dest = &d->map->tab_registers[offset];

            return Slave::readRegisters (start + pduAddressing() ? 0 : 1, dest, nb);
          }
          break;

          case ReadInputRegisters: {
            start = std::max (start, d->map->start_input_registers);
            start = std::min (start, d->map->start_input_registers + d->map->nb_input_registers - 1);
            offset = start - d->map->start_input_registers;
            nb = std::min (nb, (d->map->nb_input_registers - offset));
            uint16_t * dest = &d->map->tab_input_registers[offset];

            return Slave::readInputRegisters (start + pduAddressing() ? 0 : 1, dest, nb);
          }
          break;

          case ReportServerId: {
            d->idReport.resize (MaxPduLength, 0);

            nb = Slave::reportSlaveId (d->idReport.size(), d->idReport.data());
            if (nb < 0) {

              d->idReport.resize (nb);
            }
            return nb;
          }
          break;

          case ReadWriteMultipleRegisters:
            // TODO
            break;

          default:
            break;
        }
      }
      return 0;
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::readFromDevice (const Request & req) {

    return readFromDevice (&req);
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::writeToDevice (const Request * req) {

    if (req) {
      PIMP_D (BufferedSlave);

      if (isOpen()) {
        int offset;
        int start = req->startingAddress();
        int nb = req->quantity();

        switch (req->function()) {

          case WriteSingleCoil: {
            start = std::max (start, d->map->start_bits);
            start = std::min (start, d->map->start_bits + d->map->nb_bits - 1);
            offset = start - d->map->start_bits;

            return Slave::writeCoil (start + pduAddressing() ? 0 : 1, d->map->tab_bits[offset] != 0);
          }
          break;

          case WriteMultipleCoils: {
            start = std::max (start, d->map->start_bits);
            start = std::min (start, d->map->start_bits + d->map->nb_bits - 1);
            offset = start - d->map->start_bits;
            nb = std::min (nb, (d->map->nb_bits - offset));
            bool * src = reinterpret_cast <bool *> (&d->map->tab_bits[offset]);

            return Slave::writeCoils (start + pduAddressing() ? 0 : 1, src, nb);
          }
          break;

          case WriteSingleRegister: {
            start = std::max (start, d->map->start_registers);
            start = std::min (start, d->map->start_registers + d->map->nb_registers - 1);
            offset = start - d->map->start_registers;

            return Slave::writeRegister (start + pduAddressing() ? 0 : 1, d->map->tab_registers[offset]);
          }
          break;

          case WriteMultipleRegisters: {
            start = std::max (start, d->map->start_registers);
            start = std::min (start, d->map->start_registers + d->map->nb_registers - 1);
            offset = start - d->map->start_registers;
            nb = std::min (nb, (d->map->nb_registers - offset));
            uint16_t * src = &d->map->tab_registers[offset];

            return Slave::writeRegisters (start + pduAddressing() ? 0 : 1, src, nb);
          }
          break;

          case ReadWriteMultipleRegisters:
            // TODO
            break;

          default:
            break;
        }
      }
      return 0;
    }
    errno = EINVAL;
    return -1;
  }


  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::readCoils (int addr, bool * dest, int nb) {
    PIMP_D (BufferedSlave);

    int pduAddr = pduAddress (addr);

    if ( (pduAddr >= d->map->start_bits) &&
         (pduAddr < (d->map->start_bits + d->map->nb_bits))) {
      int offset = pduAddr - d->map->start_bits;
      uint8_t * src = &d->map->tab_bits[offset];

      nb = std::min (nb, (d->map->nb_bits - offset));

      if (isOpen()) {
        int rc = Slave::readCoils (addr, (bool *) src, nb);
        if (rc < 0) {

          return rc;
        }
      }
      memcpy (dest, src, nb * sizeof (dest[0]));
      return nb;
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::readDiscreteInputs (int addr, bool * dest, int nb) {
    PIMP_D (BufferedSlave);

    int pduAddr = pduAddress (addr);

    if ( (pduAddr >= d->map->start_input_bits) &&
         (pduAddr < (d->map->start_input_bits + d->map->nb_input_bits))) {
      int offset = pduAddr - d->map->start_input_bits;
      uint8_t * src = &d->map->tab_input_bits[offset];

      nb = std::min (nb, (d->map->nb_input_bits - offset));

      if (isOpen()) {
        int rc = Slave::readDiscreteInputs (addr, (bool *) src, nb);
        if (rc < 0) {

          return rc;
        }
      }
      memcpy (dest, src, nb * sizeof (dest[0]));
      return nb;

    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::readRegisters (int addr, uint16_t * dest, int nb) {
    PIMP_D (BufferedSlave);

    int pduAddr = pduAddress (addr);

    if ( (pduAddr >= d->map->start_registers) &&
         (pduAddr < (d->map->start_registers + d->map->nb_registers))) {
      int offset = pduAddr - d->map->start_registers;
      uint16_t * src = &d->map->tab_registers[offset];

      nb = std::min (nb, (d->map->nb_registers - offset));

      if (isOpen()) {
        int rc = Slave::readRegisters (addr, src, nb);
        if (rc < 0) {

          return rc;
        }
      }
      memcpy (dest, src, nb * sizeof (dest[0]));
      return nb;
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::readInputRegisters (int addr, uint16_t * dest, int nb) {
    PIMP_D (BufferedSlave);

    int pduAddr = pduAddress (addr);

    if ( (pduAddr >= d->map->start_input_registers) &&
         (pduAddr < (d->map->start_input_registers + d->map->nb_input_registers))) {
      int offset = pduAddr - d->map->start_input_registers;
      uint16_t * src = &d->map->tab_input_registers[offset];

      nb = std::min (nb, (d->map->nb_input_registers - offset));

      if (isOpen()) {
        int rc = Slave::readInputRegisters (addr, src, nb);
        if (rc < 0) {

          return rc;
        }
      }

      memcpy (dest, src, nb * sizeof (dest[0]));
      return nb;
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::writeCoils (int addr, const bool * src, int nb) {
    PIMP_D (BufferedSlave);

    int pduAddr = pduAddress (addr);

    if ( (pduAddr >= d->map->start_bits) &&
         (pduAddr < (d->map->start_bits + d->map->nb_bits)))  {
      int offset = pduAddr - d->map->start_bits;
      uint8_t * dest = &d->map->tab_bits[offset];

      nb = std::min (nb, (d->map->nb_bits - offset));

      memcpy (dest, src, nb * sizeof (dest[0]));
      if (isOpen()) {

        if (nb == 1) {

          return Slave::writeCoil (addr, dest[0] != 0);
        }
        else {

          return  Slave::writeCoils (addr, (bool *) dest, nb);
        }
      }

      return nb;
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::writeCoil (int addr, bool value) {

    return writeCoils (addr, &value, 1);
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::writeRegisters (int addr, const uint16_t * src, int nb) {
    PIMP_D (BufferedSlave);

    int pduAddr = pduAddress (addr);

    if ( (pduAddr >= d->map->start_registers) &&
         (pduAddr < (d->map->start_registers + d->map->nb_registers)))  {
      int offset = pduAddr - d->map->start_registers;
      uint16_t * dest = &d->map->tab_registers[offset];

      nb = std::min (nb, (d->map->nb_registers - offset));

      memcpy (dest, src, nb * sizeof (dest[0]));
      if (isOpen()) {
        if (nb == 1) {

          return Slave::writeRegister (addr, dest[0]);
        }
        else {

          return  Slave::writeRegisters (addr, dest, nb);
        }
      }
      return nb;
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::writeRegister (int addr, uint16_t value) {

    return writeRegisters (addr, &value, 1);
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::writeReadRegisters (int write_addr, const uint16_t * write_src, int write_nb,
                                         int read_addr, uint16_t * read_dest, int read_nb) {
    PIMP_D (BufferedSlave);

    int pduReadAddr = pduAddress (read_addr);
    int pduWriteAddr = pduAddress (write_addr);

    if ( (pduReadAddr >= d->map->start_registers) &&
         (pduWriteAddr >= d->map->start_registers) &&
         (pduReadAddr < (d->map->start_registers + d->map->nb_registers)) &&
         (pduWriteAddr < (d->map->start_registers + d->map->nb_registers)))  {
      int offsetRead = pduReadAddr - d->map->start_registers;
      int offsetWrite = pduWriteAddr - d->map->start_registers;
      uint16_t * destRead = &d->map->tab_registers[offsetRead];
      uint16_t * srcWrite = &d->map->tab_registers[offsetWrite];

      read_nb = std::min (read_nb, (d->map->nb_registers - offsetRead));
      write_nb = std::min (write_nb, (d->map->nb_registers - offsetWrite));

      memcpy (srcWrite, write_src, write_nb * sizeof (srcWrite[0]));
      if (isOpen()) {

        read_nb =  Slave::writeReadRegisters (write_addr, srcWrite, write_nb,
                                              read_addr, destRead, read_nb);
      }

      if (read_nb >= 0) {

        memcpy (read_dest, destRead, read_nb * sizeof (srcWrite[0]));
      }
      return read_nb;

    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  // overload
  int BufferedSlave::reportSlaveId (uint16_t max_dest, uint8_t * dest) {
    PIMP_D (BufferedSlave);

    if (isOpen()) {

      d->idReport.resize (MaxPduLength, 0);

      int rc = Slave::reportSlaveId (d->idReport.size(), d->idReport.data());
      if (rc < 0) {

        return rc;
      }
      d->idReport.resize (rc);
    }
    max_dest = std::min (max_dest, static_cast<uint16_t> (d->idReport.size()));
    memcpy (dest, d->idReport.data(), max_dest);
    return max_dest;
  }

// ---------------------------------------------------------------------------
  int BufferedSlave::writeDiscreteInputs (int addr, const bool * src, int nb) {
    PIMP_D (BufferedSlave);

    addr = pduAddress (addr);

    if ( (addr >= d->map->start_input_bits) &&
         (addr < (d->map->start_input_bits + d->map->nb_input_bits)))  {
      int offset = addr - d->map->start_input_bits;
      uint8_t * dest = &d->map->tab_input_bits[offset];

      nb = std::min (nb, (d->map->nb_input_bits - offset));
      memcpy (dest, src, nb * sizeof (dest[0]));
      return nb;
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::writeDiscreteInput (int addr, bool value) {

    return writeDiscreteInputs (addr, &value, 1);
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::writeInputRegisters (int addr, const uint16_t * src, int nb) {
    PIMP_D (BufferedSlave);

    addr = pduAddress (addr);

    if ( (addr >= d->map->start_input_registers) &&
         (addr < (d->map->start_input_registers + d->map->nb_input_registers)))  {
      int offset = addr - d->map->start_input_registers;
      uint16_t * dest = &d->map->tab_input_registers[offset];

      nb = std::min (nb, (d->map->nb_input_registers - offset));
      memcpy (dest, src, nb * sizeof (dest[0]));
      return nb;
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::writeInputRegister (int addr, uint16_t value) {

    return writeInputRegisters (addr, &value, 1);
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::updateSlaveFromBlock (Table t) {
    PIMP_D (BufferedSlave);

    switch (t) {
      case Coil:
        return d->updateSlaveCoilFromBlock();
      case HoldingRegister:
        return d->updateSlaveHoldingRegisterFromBlock();
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  bool BufferedSlave::updateSlaveFromBlock () {

    return updateSlaveFromBlock (Coil) >= 0 &&
           updateSlaveFromBlock (HoldingRegister) >= 0;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::updateBlockFromSlave (Table t) {
    PIMP_D (BufferedSlave);

    switch (t) {
      case DiscreteInput:
        return d->updateDiscreteInputBlockFromSlave();
      case Coil:
        return d->updateCoilBlockFromSlave();
      case InputRegister:
        return d->updateInputRegisterBlockFromSlave();
      case HoldingRegister:
        return d->updateHoldingRegisterBlockFromSlave();
    }
    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  bool BufferedSlave::updateBlockFromSlave () {

    return updateBlockFromSlave (Coil) >= 0 &&
           updateBlockFromSlave (HoldingRegister) >= 0 &&
           updateBlockFromSlave (DiscreteInput) >= 0 &&
           updateBlockFromSlave (InputRegister) >= 0;
  }

  // ---------------------------------------------------------------------------
  // protected
  modbus_mapping_t * BufferedSlave::map() {
    PIMP_D (BufferedSlave);

    return d->map;
  }

  // ---------------------------------------------------------------------------
  // protected
  const modbus_mapping_t * BufferedSlave::map() const {
    PIMP_D (const BufferedSlave);

    return d->map;
  }

  // ---------------------------------------------------------------------------
  //
  //                         BufferedSlave::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  BufferedSlave::Private::Private (BufferedSlave * q) :
    Slave::Private (q), map (modbus_mapping_new (0, 0, 0, 0)),
    beforeReplyCB (0), afterReplyCB (0) {

  }

  // ---------------------------------------------------------------------------
  BufferedSlave::Private::Private (BufferedSlave * q, int s, Device * d) :
    Private (q) {

    id = s;
    dev = d;
  }

  // ---------------------------------------------------------------------------
  BufferedSlave::Private::~Private() {

    modbus_mapping_free (map);
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::updateDiscreteInputBlockFromSlave() {
    PIMP_Q (Slave);

    int nb = map->nb_input_bits;
    if (q->isOpen() && nb > 0) {
      int addr = q->dataAddress (map->start_input_bits);
      uint8_t * dest = map->tab_input_bits;

      return  q->readDiscreteInputs (addr, (bool *) dest, nb);
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::updateCoilBlockFromSlave() {
    PIMP_Q (Slave);

    int nb = map->nb_bits;
    if (q->isOpen() && nb > 0) {
      int addr = q->dataAddress (map->start_bits);
      uint8_t * dest = map->tab_bits;

      return  q->readCoils (addr, (bool *) dest, nb);
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::updateHoldingRegisterBlockFromSlave() {
    PIMP_Q (Slave);

    int nb = map->nb_registers;
    if (q->isOpen() && nb > 0) {
      int addr = q->dataAddress (map->start_registers);
      uint16_t * dest = map->tab_registers;

      return  q->readRegisters (addr, dest, nb);
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::updateInputRegisterBlockFromSlave() {
    PIMP_Q (Slave);

    int nb = map->nb_input_registers;
    if (q->isOpen() && nb > 0) {
      int addr = q->dataAddress (map->start_input_registers);
      uint16_t * dest = map->tab_input_registers;

      return  q->readInputRegisters (addr, dest, nb);
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::updateSlaveCoilFromBlock() {
    PIMP_Q (Slave);

    int nb = map->nb_bits;
    if (q->isOpen() && nb > 0) {
      int addr = q->dataAddress (map->start_bits);
      uint8_t * src = map->tab_bits;

      if (nb == 1) {

        return q->writeCoil (addr, src[0] != 0);
      }
      else {

        return  q->writeCoils (addr, (bool *) src, nb);
      }
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::updateSlaveHoldingRegisterFromBlock() {
    PIMP_Q (Slave);

    int nb = map->nb_registers;
    if (q->isOpen() && nb > 0) {
      int addr = q->dataAddress (map->start_registers);
      uint16_t * src = map->tab_registers;

      if (nb == 1) {

        return q->writeRegister (addr, src[0]);
      }
      else {

        return  q->writeRegisters (addr, src, nb);
      }
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::setCoilBlock (int addr, int nmemb) {
    PIMP_Q (BufferedSlave);

    if (nmemb == 0) {

      if (map->tab_bits) {

        free (map->tab_bits);
      }
      map->tab_bits = NULL;
    }
    else {  // nmemb != 0

      if (map->nb_bits != nmemb) {

        if (! map->tab_bits) {

          map->tab_bits = (uint8_t *) calloc (nmemb, 1);
        }
        else {

          map->tab_bits = (uint8_t *) realloc (map->tab_bits, nmemb);
          if ( (map->tab_bits) && (nmemb > map->nb_bits)) {

            memset (&map->tab_bits[map->nb_bits], 0, nmemb - map->nb_bits);
          }
        }

        if (map->tab_bits == NULL) {
          // ENOMEM
          return -1;
        }
      }
    }

    map->nb_bits = nmemb;
    map->start_bits = q->pduAddress (addr);
    return nmemb;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::setDiscreteInputBlock (int addr, int nmemb) {
    PIMP_Q (BufferedSlave);

    if (nmemb == 0) {

      if (map->tab_input_bits) {

        free (map->tab_input_bits);
      }
      map->tab_input_bits = NULL;
    }
    else {  // nmemb != 0

      if (map->nb_input_bits != nmemb) {

        if (! map->tab_input_bits) {

          map->tab_input_bits = (uint8_t *) calloc (nmemb, 1);
        }
        else {

          map->tab_input_bits = (uint8_t *) realloc (map->tab_input_bits, nmemb);
          if ( (map->tab_input_bits) && (nmemb > map->nb_input_bits)) {

            memset (&map->tab_input_bits[map->nb_input_bits], 0, nmemb - map->nb_input_bits);
          }
        }

        if (map->tab_input_bits == NULL) {
          // ENOMEM
          return -1;
        }
      }
    }

    map->nb_input_bits = nmemb;
    map->start_input_bits = q->pduAddress (addr);
    return nmemb;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::setHoldingRegisterBlock (int addr, int nmemb) {
    PIMP_Q (BufferedSlave);

    if (nmemb == 0) {

      if (map->tab_registers) {

        free (map->tab_registers);
      }
      map->tab_registers = NULL;
    }
    else {  // nmemb != 0

      if (map->nb_registers != nmemb) {

        if (! map->tab_registers) {

          map->tab_registers = (uint16_t *) calloc (nmemb, sizeof (uint16_t));
        }
        else {

          map->tab_registers = (uint16_t *) realloc (map->tab_registers,
                               nmemb * sizeof (uint16_t));

          if ( (map->tab_registers) && (nmemb > map->nb_registers)) {

            memset (&map->tab_registers[map->nb_registers * sizeof (uint16_t)],
                    0, (nmemb - map->nb_registers) * sizeof (uint16_t));
          }
        }

        if (map->tab_registers == NULL) {
          // ENOMEM
          return -1;
        }
      }
    }

    map->nb_registers = nmemb;
    map->start_registers = q->pduAddress (addr);
    return nmemb;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::Private::setInputRegisterBlock (int addr, int nmemb) {
    PIMP_Q (BufferedSlave);

    if (nmemb == 0) {

      if (map->tab_input_registers) {

        free (map->tab_input_registers);
      }
      map->tab_input_registers = NULL;
    }
    else {  // nmemb != 0

      if (map->nb_input_registers != nmemb) {

        if (! map->tab_input_registers) {

          map->tab_input_registers = (uint16_t *) calloc (nmemb, sizeof (uint16_t));
        }
        else {

          map->tab_input_registers = (uint16_t *) realloc (map->tab_input_registers,
                                     nmemb * sizeof (uint16_t));

          if ( (map->tab_input_registers) && (nmemb > map->nb_input_registers)) {

            memset (&map->tab_input_registers[map->nb_input_registers * sizeof (uint16_t)],
                    0, (nmemb - map->nb_input_registers) * sizeof (uint16_t));
          }
        }

        if (map->tab_input_registers == NULL) {
          // ENOMEM
          return -1;
        }
      }
    }

    map->nb_input_registers = nmemb;
    map->start_input_registers = q->pduAddress (addr);
    return nmemb;
  }

  // ---------------------------------------------------------------------------
  //
  //                         Modbus::Json Namespace
  //
  // ---------------------------------------------------------------------------
  namespace Json {

    // -------------------------------------------------------------------------
    void setConfig (BufferedSlave * s, const nlohmann::json & j) {

      setConfig (reinterpret_cast<Slave *> (s), j);
      if (j.contains ("blocks")) {

        auto blocks = j["blocks"];
        for (const auto & block : blocks) {
          int startAddr = s->dataAddress (0);
          DataType dt (Uint16);
          auto table = block["table"].get<Table>(); // mandatory
          auto nmemb = block["quantity"].get<int>(); // mandatory

          if (block.contains ("starting-address")) {

            block["starting-address"].get_to (startAddr);
          }

          if (block.contains ("data-type")) {

            //auto de = block["data-type"].get<DataEnum> ();
            //dt = de;
            block["data-type"].get_to (dt.value());
            nmemb *= dt.size() / sizeof (uint16_t);
          }

          s->setBlock (table, nmemb, startAddr);

          if (block.contains ("values")) {

            if (table == InputRegister || table == HoldingRegister) {

              Json::writeRegisters (s, block);
            }
            else {

              // Coils and discrete inputs
              Json::writeBits (s, block);
            }
            //---- values
          }

        }
      }
    }


    //--------------------------------------------------------------------------
    int writeBits (BufferedSlave * s, const nlohmann::json & block) {
      int rc = 0;
      const auto & values = block["values"];
      auto nmemb = block["quantity"].get<int>(); // mandatory

      if (values.is_array() && values.size() > 0 && nmemb > 0) {
        std::vector<uint8_t> vect;
        auto table = block["table"].get<Table>(); // mandatory
        int addr = s->dataAddress (0);

        if (block.contains ("values-address")) {

          block["values-address"].get_to (addr);
        }

        if (getBitValues (vect, nmemb, values) > 0) {
          if (table == Coil) {

            rc = s->writeCoils (addr, (bool *) vect.data(), vect.size());
          }
          else {

            rc = s->writeDiscreteInputs (addr, (bool *) vect.data(), vect.size());
          }
        }
      }

      return rc;
    }

    //--------------------------------------------------------------------------
    int writeRegisters (BufferedSlave * s, const nlohmann::json & block) {
      DataType dt (Uint16);

      if (block.contains ("data-type")) {

        block["data-type"].get_to (dt.value());
      }

      switch (dt.value()) {
        case Uint16:
          break;
        case Uint32:
          return Json::writeRegisters<uint32_t> (s, block);
          break;
        case Uint64:
          return Json::writeRegisters<uint64_t> (s, block);
          break;
        case Int16:
          return Json::writeRegisters<int16_t> (s, block);
          break;
        case Int32:
          return Json::writeRegisters<int32_t> (s, block);
          break;
        case Int64:
          return Json::writeRegisters<int64_t> (s, block);
          break;
        case Float:
          return Json::writeRegisters<float> (s, block);
          break;
        case Double:
          return Json::writeRegisters<double> (s, block);
          break;
        case LongDouble:
          return Json::writeRegisters<long double> (s, block);
          break;
      }
      return Json::writeRegisters<uint16_t> (s, block);
    }

    //--------------------------------------------------------------------------
    int getBitValues (std::vector<uint8_t> & data, int nmemb,
                      const nlohmann::json & values) {

      for (const auto & v : values) {

        if (v.is_string()) {
          size_t idx;
          auto str = v.get<std::string>();

          unsigned long ul = stoul (str, &idx, 0);
          if (idx < str.size() || ul > std::numeric_limits<uint8_t>::max()) {
            throw std::invalid_argument (
              "Cannot convert " + str +
              " to a byte, value must be between 0 and 0xFF");
          }

          int m = std::min (nmemb, 8);
          uint8_t byte = ul;

          for (int i = 0; i < m; i++) {
            uint8_t b = byte & (1 << i) ? true : false;
            data.push_back (b);
            nmemb--;
          }
        }
        else { // numeric type
          bool b;

          try {
            // try to convert to bool : true/false
            v.get_to (b);
          }
          catch (nlohmann::json::exception& e) {
            // unable to convert to bool, try with 0/1
            int i;
            v.get_to (i);
            if (i != 0 && i != 1) {
              throw std::invalid_argument (
                std::to_string (i) +
                " not a binary value (must be 0/1 or true/false)");
            }
            b = (i != 0);
          }
          data.push_back (b);
          nmemb--;
        }

        if (nmemb <= 0) {
          break;
        }
      }
      return data.size();
    }
  }
}
/* ========================================================================== */
