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
    return -1;
  }

  // ---------------------------------------------------------------------------
  int BufferedSlave::writeInputRegister (int addr, uint16_t value) {

    return writeInputRegisters (addr, &value, 1);
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
        for (const auto & b : blocks) {
          auto t = b["table"].get<Table>();
          auto nmemb = b["quantity"].get<int>();
          int startAddr = -1;

          if (j.contains ("starting-address")) {

            auto i = b["starting-address"].get<int>();
            startAddr = i;
          }

          s->setBlock (t, nmemb, startAddr);
        }
      }
    }
  }
}
/* ========================================================================== */
