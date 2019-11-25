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
#include <cstring>
#include <modbuspp/device.h>
#include <modbuspp/netlayer.h>
#include "message_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                          Message Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Message::Message (Message::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Message::Message (NetLayer & backend) :
    d_ptr (new Private (this, &backend)) {}

  // ---------------------------------------------------------------------------
  Message::Message (Device & dev) :
    Message (dev.backend()) {}

  // ---------------------------------------------------------------------------
  Message::Message (NetLayer & backend, const std::vector<uint8_t> & adu) :
    d_ptr (new Private (this, &backend, adu)) {}

  // ---------------------------------------------------------------------------
  Message::Message (Device & dev, const std::vector<uint8_t> & adu) :
    Message (dev.backend(), adu) {}

  // ---------------------------------------------------------------------------
  Message::Message (NetLayer & backend, Function f) :
    d_ptr (new Private (this, &backend, f)) {}

  // ---------------------------------------------------------------------------
  Message::Message (Device & dev, Function f) :
    Message (dev.backend(), f) {}

  // ---------------------------------------------------------------------------
  Message::~Message() = default;

  // ---------------------------------------------------------------------------
  uint16_t Message::maxAduLength() const {
    PIMP_D (const Message);

    return d->adu.capacity();
  }

  // ---------------------------------------------------------------------------
  Net Message::net() const {
    PIMP_D (const Message);

    return d->net;
  }

  // ---------------------------------------------------------------------------
  std::vector<uint8_t> & Message::adu () {
    PIMP_D (Message);

    return d->adu;
  }

  // ---------------------------------------------------------------------------
  const std::vector<uint8_t> & Message::adu () const {
    PIMP_D (const Message);

    return d->adu;
  }

  // ---------------------------------------------------------------------------
  uint8_t Message::byte (int i) const {
    PIMP_D (const Message);

    return d->adu[d->pduIndex + i];
  }

  // ---------------------------------------------------------------------------
  uint16_t Message::word (int i) const {
    PIMP_D (const Message);

    return (d->adu[d->pduIndex + i] << 8) + d->adu[d->pduIndex + i + 1];
  }

  // ---------------------------------------------------------------------------
  void Message::setWord (int pduOffset, uint16_t value) {
    PIMP_D (Message);
    
    d->adu[d->pduIndex + pduOffset] = value >> 8;
    d->adu[d->pduIndex + pduOffset + 1] = value & 0xFF;
  }
  
  // ---------------------------------------------------------------------------
  void Message::setByte (int pduOffset, uint8_t value) {
    PIMP_D (Message);

    d->adu[d->pduIndex + pduOffset] = value;
  }

  // ---------------------------------------------------------------------------
  int Message::slave() const {

    return byte (-1);
  }

  // ---------------------------------------------------------------------------
  Function Message::function() const {

    return (Function) byte (0);
  }

  // ---------------------------------------------------------------------------
  uint16_t Message::startingAddress() const {

    return word (1);
  }

  // ---------------------------------------------------------------------------
  uint16_t Message::quantity() const {

    return word (3);
  }

  // ---------------------------------------------------------------------------
  uint16_t Message::value (uint16_t i) const {

    return word (5 + i);
  }

  // ---------------------------------------------------------------------------
  void Message::setSlaveId (int id) {
    PIMP_D (Message);

    setByte (-1, static_cast<uint8_t> (id & 0xFF));
  }

  // ---------------------------------------------------------------------------
  void Message::setFunction (Function func) {
    PIMP_D (Message);

    setByte (0, static_cast<uint8_t> (func));
  }

  // ---------------------------------------------------------------------------
  void Message::setStartingAdress (uint16_t addr) {

    setWord (1, addr);
  }

  // ---------------------------------------------------------------------------
  void Message::setQuantity (uint16_t n) {

    setWord (3, n);
  }

  // ---------------------------------------------------------------------------
  //
  //                         Message::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Message::Private::Private (Message * q, NetLayer * b) :
    q_ptr (q), net (b->net()), pduIndex (modbus_get_header_length (b->context())) {

    adu.resize (b->maxAduLength(), 0);
  }

  // ---------------------------------------------------------------------------
  Message::Private::Private (Message * q, NetLayer * b, const std::vector<uint8_t> & m) :
    Private (q, b) {

    adu = m;
  }

  // ---------------------------------------------------------------------------
  Message::Private::Private (Message * q, NetLayer * b, Function func) :
    Private (q, b) {

    adu[pduIndex] = static_cast<uint8_t> (func);
  }

  // ---------------------------------------------------------------------------
  Message::Private::~Private() = default;
}

/* ========================================================================== */
