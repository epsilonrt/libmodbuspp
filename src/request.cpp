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
#include <modbuspp/device.h>
#include <modbuspp/netlayer.h>
#include <modbuspp/request.h>
#include "request_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                          Request Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Request::Request (Request::Private &dd) : Message (dd) {}

  // ---------------------------------------------------------------------------
  Request::Request (NetLayer & backend) :
    Message (*new Private (this, &backend)) {}

  // ---------------------------------------------------------------------------
  Request::Request (NetLayer & backend, const std::vector<uint8_t> & adu) :
    Message (*new Private (this, &backend, adu)) {}

  // ---------------------------------------------------------------------------
  Request::Request (NetLayer & backend, Function f) :
    Message (*new Private (this, &backend, f)) {}

  // ---------------------------------------------------------------------------
  Request::Request (Device & dev) :
    Request (dev.backend()) {}

  // ---------------------------------------------------------------------------
  Request::Request (Device & dev, const std::vector<uint8_t> & adu) :
    Request (dev.backend(), adu) {}

  // ---------------------------------------------------------------------------
  Request::Request (NetLayer & backend, const uint8_t * adu, size_t len) :
    Request (backend, std::vector<uint8_t> (adu, adu + len)) {}

  // ---------------------------------------------------------------------------
  Request::Request (Device & dev, const uint8_t * adu, size_t len) :
    Request (dev, std::vector<uint8_t> (adu, adu + len)) {}

  // ---------------------------------------------------------------------------
  Request::Request (Device & dev, Function f) :
    Request (dev.backend(), f) {}

  // ---------------------------------------------------------------------------
  Request::Request (Net net) :
    Message (*new Private (this, net)) {}

  // ---------------------------------------------------------------------------
  Request::Request (Net net, const std::vector<uint8_t> & adu) :
    Message (*new Private (this, net, adu)) {}

  // ---------------------------------------------------------------------------
  Request::Request (Net net, const uint8_t * adu, size_t len) :
    Request (net, std::vector<uint8_t> (adu, adu + len)) {}

  // ---------------------------------------------------------------------------
  Request::Request (Net net, Function f) :
    Message (*new Private (this, net, f)) {}

  // ---------------------------------------------------------------------------
  Request::Request (const Request & other) :
    Message (other) {}

  // ---------------------------------------------------------------------------
  Request::Request (Request && other) :
    Message (other) {}

  // ---------------------------------------------------------------------------
  uint16_t Request::registerValue () const {

    return word (3);
  }

  // ---------------------------------------------------------------------------
  uint16_t Request::registerValue (uint16_t index) const {

    return word (6 + index * 2);
  }

  // ---------------------------------------------------------------------------
  void Request::registerValues (uint16_t index, uint16_t quantity, uint16_t * values) const {

    for (uint16_t i = 0; i < quantity; i++) {

      values[i] = registerValue (index + i);
    }
  }

  // ---------------------------------------------------------------------------
  bool Request::coilValue () const {

    return word (3) == 0xFF00;
  }

  // ---------------------------------------------------------------------------
  bool Request::coilValue (uint16_t index) const {
    uint8_t bit = index % 8;

    index /= 8;
    return (byte (index + 6) & (1 << bit)) != 0;
  }

  // ---------------------------------------------------------------------------
  void Request::coilValues (uint16_t index, uint16_t quantity, bool * values) const {

    for (uint16_t i = 0; i < quantity; i++) {

      values[i] = coilValue (i + index);
    }
  }

  // ---------------------------------------------------------------------------
  uint8_t Request::byteCount () const {

    return byte (5);
  }

  // ---------------------------------------------------------------------------
  void Request::setByteCount (uint8_t n) {

    setByte (5, n);
  }

  // ---------------------------------------------------------------------------
  void Request::setRegisterValue (uint16_t value) {

    setWord (3, value);
  }

  // ---------------------------------------------------------------------------
  void Request::setRegisterValue (uint16_t index, uint16_t value) {

    setWord (6 + index * 2, value);
  }

  // ---------------------------------------------------------------------------
  void Request::setRegisterValues (uint16_t index, uint16_t quantity, const uint16_t * values) {

    for (uint16_t i = 0; i < quantity; i++) {

      setRegisterValue (index + i, values[i]);
    }
  }

  // ---------------------------------------------------------------------------
  void Request::setCoilValue (bool value) {

    setWord (3, value ? 0xFF00 : 0x0000);
  }

  // ---------------------------------------------------------------------------
  void Request::setCoilValue (uint16_t index, bool value) {
    uint8_t b, bit = index % 8;

    index /= 8;
    b = byte (index + 6);
    b &= ~ (1 << bit);
    b |= (value ? 1 : 0) << bit;
    setByte (index + 6, b);
  }

  // ---------------------------------------------------------------------------
  void Request::setCoilValues (uint16_t index, uint16_t quantity, const bool * values)  {

    for (uint16_t i = 0; i < quantity; i++) {

      setCoilValue (index + i, values[i]);
    }
  }

  // ---------------------------------------------------------------------------
  //
  //                         Request::Private Class
  //
  // ---------------------------------------------------------------------------
  
  // ---------------------------------------------------------------------------
  Request::Private::Private (Request * q, NetLayer * b) :
    Message::Private (q, b) {}

  // ---------------------------------------------------------------------------
  Request::Private::Private (Request * q, NetLayer * b, const std::vector<uint8_t> & m) :
    Message::Private (q, b, m) {}

  // ---------------------------------------------------------------------------
  Request::Private::Private (Request * q, NetLayer * b, Function f) :
    Message::Private (q, b, f) {}

  // ---------------------------------------------------------------------------
  Request::Private::Private (Request * q, Net n) :
    Message::Private (q, n) {}

  // ---------------------------------------------------------------------------
  Request::Private::Private (Request * q, Net n, const std::vector<uint8_t> & m) :
    Message::Private (q, n, m) {}

  // ---------------------------------------------------------------------------
  Request::Private::Private (Request * q, Net n, Function f) :
    Message::Private (q, n, f) {}
    
  // ---------------------------------------------------------------------------
  Request::Private::~Private() = default;

}

/* ========================================================================== */
