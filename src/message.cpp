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
  Message::Message (NetLayer & backend, Function f) :
    d_ptr (new Private (this, &backend, f)) {}

  // ---------------------------------------------------------------------------
  Message::Message (Device & dev, Function f) :
    Message (dev.backend(), f) {}

  // ---------------------------------------------------------------------------
  Message::Message (const Message & other) :
    d_ptr (new Private (*other.d_ptr)) {

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
    size_t index = d->pduIndex + pduOffset;

    if ( (index + 1) >= d->adu.size()) {
      d->adu.resize (index + 2);
    }
    d->adu[index++] = value >> 8;
    d->adu[index++] = value & 0xFF;
    d->crcIndex = std::max (d->crcIndex, static_cast<int> (index));
  }

  // ---------------------------------------------------------------------------
  void Message::setByte (int pduOffset, uint8_t value) {
    PIMP_D (Message);
    size_t index = d->pduIndex + pduOffset;

    if (index >= d->adu.size()) {
      d->adu.resize (index + 1);
    }
    d->adu[index++] = value;
    d->crcIndex = std::max (d->crcIndex, static_cast<int> (index));
  }

  // ---------------------------------------------------------------------------
  void Message::setCrc (uint16_t pduOffset) {
    PIMP_D (Message);
    size_t index = pduOffset > 0 ? d->pduIndex + pduOffset : d->crcIndex;
    uint16_t crc = d->crc16 (d->adu.data(), index);

    d->adu.resize (index + 2);
    d->adu[index++] = crc >> 8;
    d->adu[index] = crc & 0xFF;
  }

  // ---------------------------------------------------------------------------
  void Message::clear() {
    PIMP_D (Message);

    d->adu.resize (d->maxAduLength, 0);
    d->crcIndex = -1;
  }

  // ---------------------------------------------------------------------------
  size_t Message::size() const {
    PIMP_D (const Message);

    return d->adu.size();
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
  void Message::print (std::ostream& os) const {
    PIMP_D (const Message);
    std::ostringstream oss;

    for (auto b : d->adu) {

      oss << d->prefix << std::uppercase << std::setfill ('0') << std::setw (2)
          << std::hex << static_cast<unsigned int> (b) << d->suffix;
    }
    os << oss.str();
  }

  // ---------------------------------------------------------------------------
  char Message::suffix() const {
    PIMP_D (const Message);

    return d->suffix;
  }

  // ---------------------------------------------------------------------------
  char Message::prefix() const {
    PIMP_D (const Message);

    return d->prefix;
  }

  // ---------------------------------------------------------------------------
  void Message::setSuffix (char s) {
    PIMP_D (Message);

    d->suffix = s;
  }

  // ---------------------------------------------------------------------------
  void Message::setPrefix (char p) {
    PIMP_D (Message);

    d->prefix = p;
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
    q_ptr (q), net (b->net()), pduIndex (modbus_get_header_length (b->context())),
    crcIndex (-1), maxAduLength (b->maxAduLength()), suffix (' '), prefix (' ') {

    adu.resize (maxAduLength, 0);
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

  // ---------------------------------------------------------------------------
  // static
  uint16_t Message::Private::crc16 (uint8_t *buffer, uint16_t buffer_length) {
    uint8_t crcHi = 0xFF; /* high CRC byte initialized */
    uint8_t crcLo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
      i = crcHi ^ *buffer++; /* calculate the CRC  */
      crcHi = crcLo ^ CrcHiTable[i];
      crcLo = CrcLoTable[i];
    }

    return (crcHi << 8 | crcLo);
  }

  // ---------------------------------------------------------------------------
  const uint8_t Message::Private::CrcHiTable[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
  };

  // ---------------------------------------------------------------------------
  const uint8_t Message::Private::CrcLoTable[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
  };
}

/* ========================================================================== */
