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
#include <modbuspp/response.h>
#include <modbuspp/request.h>
#include "response_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                          Response Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Response::Response (Response::Private &dd) : Message (dd) {}

  // ---------------------------------------------------------------------------
  Response::Response (NetLayer & backend) :
    Message (*new Private (this, &backend)) {}

  // ---------------------------------------------------------------------------
  Response::Response (NetLayer & backend, const std::vector<uint8_t> & adu) :
    Message (*new Private (this, &backend, adu)) {}

  // ---------------------------------------------------------------------------
  Response::Response (NetLayer & backend, Function f) :
    Message (*new Private (this, &backend, f)) {}

  // ---------------------------------------------------------------------------
  Response::Response (Device & dev) :
    Response (dev.backend()) {}

  // ---------------------------------------------------------------------------
  Response::Response (Device & dev, const std::vector<uint8_t> & adu) :
    Response (dev.backend(), adu) {}

  // ---------------------------------------------------------------------------
  Response::Response (NetLayer & backend, const uint8_t * adu, size_t len) :
    Response (backend, std::vector<uint8_t> (adu, adu + len)) {}

  // ---------------------------------------------------------------------------
  Response::Response (Device & dev, const uint8_t * adu, size_t len) :
    Response (dev, std::vector<uint8_t> (adu, adu + len)) {}

  // ---------------------------------------------------------------------------
  Response::Response (Device & dev, Function f) :
    Response (dev.backend(), f) {}

  // ---------------------------------------------------------------------------
  Response::Response (const Request & req) :
    Message (req) {
    PIMP_D(Response);
    
    d->isResponse = true;
  }
  
  // ---------------------------------------------------------------------------
  Response::Response (const Message & msg) :
    Message (msg) {
    PIMP_D(Response);
    
    d->isResponse = true;
  }

  // ---------------------------------------------------------------------------
  Response::Response (const Response & other) :
    Message (other) {}

  // ---------------------------------------------------------------------------
  Response::Response (Request && req) :
    Message (req) {
    PIMP_D(Response);
    
    d->isResponse = true;
  }
  
  // ---------------------------------------------------------------------------
  Response::Response (Message && msg) :
    Message (msg) {
    PIMP_D(Response);
    
    d->isResponse = true;
  }

  // ---------------------------------------------------------------------------
  Response::Response (Response && other) :
    Message (other) {}

  // ---------------------------------------------------------------------------
  void Response::setByteCount (uint8_t n) {

    setByte (1, n);
  }

  // ---------------------------------------------------------------------------
  void Response::setRegisterValue (uint16_t index, uint16_t value) {

    setWord (2 + index * 2, value);
  }

  // ---------------------------------------------------------------------------
  void Response::setRegisterValues (uint16_t index, uint16_t quantity, const uint16_t * value) {

    for (uint16_t i = 0; i < quantity; i++) {

      setRegisterValue (index + i, value[i]);
    }
  }

  // ---------------------------------------------------------------------------
  void Response::setBitValue (uint16_t index, bool value) {
    uint8_t b, bit = index % 8;

    index /= 8;
    b = byte (index);
    b &= ~ (1 << bit);
    b |= (value ? 1 : 0) << bit;
    setByte (index, b);
  }

  // ---------------------------------------------------------------------------
  void Response::setBitValues (uint16_t index, uint16_t quantity, const bool * values)  {

    for (uint16_t i = 0; i < quantity; i++) {

      setBitValue (index + i, values[i]);
    }
  }

  // ---------------------------------------------------------------------------
  void Response::setExceptionCode (ExceptionCode code) {

    setFunction (static_cast<Function> (function() | ExceptionFlag));
    setByte (1, code);
  }

  // ---------------------------------------------------------------------------
  ExceptionCode Response::exceptionCode() const {
    
    return static_cast<ExceptionCode> (byte (1));
  }

  // ---------------------------------------------------------------------------
  uint16_t Response::registerValue (uint16_t index) const {

    return word (2 + index * 2);
  }

  // ---------------------------------------------------------------------------
  void Response::registerValues (uint16_t index, uint16_t quantity, uint16_t * values) const {

    for (uint16_t i = 0; i < quantity; i++) {

      values[i] = registerValue (index + i);
    }
  }

  // ---------------------------------------------------------------------------
  bool Response::bitValue (uint16_t index) const {
    uint8_t bit = index % 8;

    index /= 8;
    return (byte (index) & (1 << bit)) != 0;
  }

  // ---------------------------------------------------------------------------
  void Response::bitValues (uint16_t index, uint16_t quantity, bool * values) const {

    for (uint16_t i = 0; i < quantity; i++) {

      values[i] = bitValue (index + i);
    }
  }


  // ---------------------------------------------------------------------------
  //
  //                         Response::Private Class
  //
  // ---------------------------------------------------------------------------
  
  // ---------------------------------------------------------------------------
  Response::Private::Private (Response * q, NetLayer * b) :
    Message::Private (q, b) {}

  // ---------------------------------------------------------------------------
  Response::Private::Private (Response * q, NetLayer * b, const std::vector<uint8_t> & m) :
    Message::Private (q, b, m) {}

  // ---------------------------------------------------------------------------
  Response::Private::Private (Response * q, NetLayer * b, Function f) :
    Message::Private (q, b, f) {}
    

}

/* ========================================================================== */
