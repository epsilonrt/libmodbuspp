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
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                          Request Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Request::Request (NetLayer & backend) :
    Message (backend) {
    setPrefix ('[');
    setSuffix (']');
  }

  // ---------------------------------------------------------------------------
  Request::Request (Device & dev) :
    Message (dev) {
    setPrefix ('[');
    setSuffix (']');
  }

  // ---------------------------------------------------------------------------
  Request::Request (NetLayer & backend, const std::vector<uint8_t> & adu) :
    Message (backend, adu) {
    setPrefix ('[');
    setSuffix (']');
  }

  // ---------------------------------------------------------------------------
  Request::Request (Device & dev, const std::vector<uint8_t> & adu) :
    Message (dev, adu) {
    setPrefix ('[');
    setSuffix (']');
  }

  // ---------------------------------------------------------------------------
  Request::Request (NetLayer & backend, Function f) :
    Message (backend, f) {
    setPrefix ('[');
    setSuffix (']');
  }

  // ---------------------------------------------------------------------------
  Request::Request (Device & dev, Function f) :
    Message (dev, f) {
    setPrefix ('[');
    setSuffix (']');
  }

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
}

/* ========================================================================== */
