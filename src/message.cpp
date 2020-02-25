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
#include <iomanip>
#include <sstream>
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
  Message::Message (NetLayer & backend, const uint8_t * adu, size_t len) :
    Message (backend, std::vector<uint8_t> (adu, adu + len)) {}

  // ---------------------------------------------------------------------------
  Message::Message (Device & dev, const uint8_t * adu, size_t len) :
    Message (dev, std::vector<uint8_t> (adu, adu + len)) {}

  // ---------------------------------------------------------------------------
  Message::Message (NetLayer & backend, Function f) :
    d_ptr (new Private (this, &backend, f)) {}

  // ---------------------------------------------------------------------------
  Message::Message (Device & dev, Function f) :
    Message (dev.backend(), f) {}

  // ---------------------------------------------------------------------------
  Message::Message (const Message & other) :
    d_ptr (new Private (*other.d_ptr)) {}

  // ---------------------------------------------------------------------------
  Message::Message (Message && other) :
    d_ptr (std::move (other.d_ptr)) {}

  // ---------------------------------------------------------------------------
  Message& Message::operator= (Message && other) {

    d_ptr = std::move (other.d_ptr);
    return *this;
  }

  // ---------------------------------------------------------------------------
  void Message::swap (Message &other) {

    d_ptr.swap (other.d_ptr);
  }

  // ---------------------------------------------------------------------------
  Message& Message::operator= (const Message &other) {

    Message (other).swap (*this);
    return *this;
  }

  // ---------------------------------------------------------------------------
  bool Message::operator== (const Message & other) {

    return other.adu() == adu();
  }

  // ---------------------------------------------------------------------------
  bool Message::operator!= (const Message & other) {

    return ! this->operator == (other);
  }

  // ---------------------------------------------------------------------------
  Message::~Message() = default;

  // ---------------------------------------------------------------------------
  uint16_t Message::maxAduLength() const {
    PIMP_D (const Message);

    return d->maxAduLength;
  }

  // ---------------------------------------------------------------------------
  Net Message::net() const {
    PIMP_D (const Message);

    return d->net;
  }

  // ---------------------------------------------------------------------------
  uint8_t * Message::adu () {
    PIMP_D (Message);

    return d->adu.data();
  }

  // ---------------------------------------------------------------------------
  const uint8_t * Message::adu () const {
    PIMP_D (const Message);

    return d->adu.data();
  }

  // ---------------------------------------------------------------------------
  uint8_t Message::byte (int i) const {
    PIMP_D (const Message);

    return d->adu[d->pduBegin + i];
  }

  // ---------------------------------------------------------------------------
  uint16_t Message::word (int i) const {
    PIMP_D (const Message);

    return (d->adu[d->pduBegin + i] << 8) + d->adu[d->pduBegin + i + 1];
  }

  // ---------------------------------------------------------------------------
  void Message::setWord (int pduOffset, uint16_t value) {
    PIMP_D (Message);
    size_t index = d->pduBegin + pduOffset;

    d->adu[index++] = value >> 8;
    d->adu[index++] = value & 0xFF;
    d->aduSize = std::max (d->aduSize, index);
  }

  // ---------------------------------------------------------------------------
  void Message::setByte (int pduOffset, uint8_t value) {
    PIMP_D (Message);
    size_t index = d->pduBegin + pduOffset;

    d->adu[index++] = value;
    d->aduSize = std::max (d->aduSize, index);
  }

  // ---------------------------------------------------------------------------
  void Message::clear() {
    PIMP_D (Message);

    d->adu.resize (d->maxAduLength, 0);
    d->aduSize = 0;
    if (d->net == Tcp) {
      setSlaveId (MODBUS_TCP_SLAVE);
    }
  }

  // ---------------------------------------------------------------------------
  size_t Message::size() const {
    PIMP_D (const Message);

    if (d->aduSize > d->pduBegin) {

      return d->aduSize - d->pduBegin;
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  void Message::setSize (size_t size) {
    PIMP_D (Message);

    d->aduSize = d->pduBegin + size;
  }

  // ---------------------------------------------------------------------------
  size_t Message::aduSize() const {
    PIMP_D (const Message);

    return d->aduSize;
  }

  // ---------------------------------------------------------------------------
  void Message::setAduSize (size_t size) {
    PIMP_D (Message);

    d->aduSize = size;
  }

  // ---------------------------------------------------------------------------
  uint16_t Message::aduHeaderLength() const {
    PIMP_D (const Message);

    return d->pduBegin;
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
  void Message::print (std::ostream& os, char prefix, char suffix) const {
    PIMP_D (const Message);
    std::ostringstream oss;

    for (size_t i = 0; i < d->aduSize; i++) {

      oss << prefix << std::uppercase << std::setfill ('0') << std::setw (2)
          << std::hex << static_cast<unsigned int> (d->adu[i]) << suffix;
    }
    os << oss.str();
  }

  // ---------------------------------------------------------------------------
  void Message::print (std::ostream& os) const {
    PIMP_D (const Message);

    print (os, d->isResponse ? '[' : '<', d->isResponse ? ']' : '>');
  }

  // ---------------------------------------------------------------------------
  bool Message::isResponse() const {
    PIMP_D (const Message);

    return d->isResponse;
  }

  // ---------------------------------------------------------------------------
  std::ostream& operator<< (std::ostream& os, const Message & m)  {

    m.print (os);
    return os;
  }

  // ---------------------------------------------------------------------------
  //
  //                         Message::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Message::Private::Private (Message * q, NetLayer * b) :
    q_ptr (q), net (b->net()), pduBegin (modbus_get_header_length (b->context())),
    aduSize (0), maxAduLength (b->maxAduLength()), isResponse (false) {

    adu.resize (maxAduLength, 0);
    if (net == Tcp) {
      adu[6] = MODBUS_TCP_SLAVE;
    }
  }

  // ---------------------------------------------------------------------------
  Message::Private::Private (Message * q, NetLayer * b, const std::vector<uint8_t> & m) :
    Private (q, b) {

    adu = m;
  }

  // ---------------------------------------------------------------------------
  Message::Private::Private (Message * q, NetLayer * b, Function func) :
    Private (q, b) {

    adu[pduBegin] = static_cast<uint8_t> (func);
  }

  // ---------------------------------------------------------------------------
  Message::Private::~Private() = default;
}

/* ========================================================================== */
