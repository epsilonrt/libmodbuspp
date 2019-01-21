/* Copyright © 2018 Pascal JEAN, All rights reserved.
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
#include "modbuspp-data_p.h"
#include "config.h"

#include <cstring>
#include <cstdio>

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                             Data Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // static
  void Data::print (const uint8_t * p, const size_t s) {

    printf ("0x");
    for (size_t i = 0; i < s; i++) {
      printf ("%02X", p[i]);
    }
    printf ("\n");
  }

  // ---------------------------------------------------------------------------
  // static
  void Data::print (const float & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }

  // ---------------------------------------------------------------------------
  // static
  void Data::print (const double & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }
  // ---------------------------------------------------------------------------
  // static
  void Data::print (const uint8_t & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }
  // ---------------------------------------------------------------------------
  // static
  void Data::print (const uint16_t & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }
  // ---------------------------------------------------------------------------
  // static
  void Data::print (const uint32_t & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }
  // ---------------------------------------------------------------------------
  // static
  void Data::print (const uint64_t & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }
  // ---------------------------------------------------------------------------
  // static
  void Data::print (const char & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }
  // ---------------------------------------------------------------------------
  // static
  void Data::print (const int & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }
  // ---------------------------------------------------------------------------
  // static
  void Data::print (const long & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }
  // ---------------------------------------------------------------------------
  // static
  void Data::print (const long long & v) {

    print ( (const uint8_t *) &v, sizeof (v));
  }

  // ---------------------------------------------------------------------------
  Data::Data (Data::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Data::Data (Type t, Endian e) :
    d_ptr (new Private (this, e, t)) {}

  // ---------------------------------------------------------------------------
  Data::Data (char i, Endian e) : Data (Byte, e) {
    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (int i, Endian e) : Data (Word, e) {
    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (long i, Endian e) : Data (LongWord, e) {
    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (long long i, Endian e) : Data (LongLongWord, e) {
    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (uint8_t i, Endian e) : Data (Byte, e) {
    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (uint16_t i, Endian e) : Data (Word, e) {
    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (uint32_t i, Endian e) : Data (LongWord, e) {

    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (uint64_t i, Endian e) : Data (LongLongWord, e) {

    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (float i, Endian e) : Data (Float, e) {

    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (double i, Endian e) : Data (Double, e) {

    set (i);
  }

  // ---------------------------------------------------------------------------
  Data::Data (const Data & other) :
    d_ptr (new Private (*other.d_ptr)) {}

  // ---------------------------------------------------------------------------
  void Data::swap (Data &other) {

    d_ptr.swap (other.d_ptr);
  }

  // ---------------------------------------------------------------------------
  Data::~Data() {

  }

  // ---------------------------------------------------------------------------
  void Data::print () const {

    print ( (const uint8_t *) ptr(), size());
  }


  // ---------------------------------------------------------------------------
  Data::Endian Data::endianness() const {
    PIMP_D (const Data);

    return d->endianness;
  }

  // ---------------------------------------------------------------------------
  uint16_t * Data::ptr() {
    PIMP_D (Data);

    return d->ptr;
  }

  // ---------------------------------------------------------------------------
  const uint16_t * Data::ptr() const {
    PIMP_D (const Data);

    return d->ptr;
  }

  // ---------------------------------------------------------------------------
  void Data::set (uint8_t uc) {
    PIMP_D (Data);
    uint16_t v = uc;

    set (v);
    d->type = Byte;
  }

  // ---------------------------------------------------------------------------
  void Data::set (uint16_t v) {
    PIMP_D (Data);

    hton (v);
    switch (d->endianness) {

      case EndianLittleBig:     // BA
      case EndianLittleLittle:  // BA
        d->ptr[0] = bswap (v);
        break;

      default:
        d->ptr[0] = v;
        break;
    }
    if (d->type == Void) {
      d->type = Word;
    }
  }

  // ---------------------------------------------------------------------------
  void Data::set (uint32_t v) {
    PIMP_D (Data);
    uint16_t * p = (uint16_t *) &v;

    hton (v);
    switch (d->endianness) { // net value: ABCD
      case EndianBigBig: // ABCD: bytes Big, word Big : no swap
        break;
      case EndianBigLittle: // CDAB: bytes Big, word Little : swap words
        std::swap (p[0], p[1]);
        break;
      case EndianLittleBig: // BADC: bytes Little, word Big : swap bytes
        p[0] = bswap (p[0]);
        p[1] = bswap (p[1]);
        break;
      case EndianLittleLittle: // DCBA: bytes Little, word Little: swap all
        v = bswap (v);
        break;
    }
    d->ptr[0] = p[0];
    d->ptr[1] = p[1];
    if (d->type == Void) {
      d->type = LongWord;
    }
  }

  // ---------------------------------------------------------------------------
  void Data::set (uint64_t v) {
    PIMP_D (Data);
    uint16_t * p = (uint16_t *) &v;

    hton (v);
    switch (d->endianness) { // net value: ABCDEFGH
      case EndianBigBig: // ABCDEFGH: bytes Big, word Big : no swap
        break;
      case EndianBigLittle: // GHEFCDAB: bytes Big, word Little : swap words
        std::swap (p[0], p[3]);
        std::swap (p[1], p[2]);
        break;
      case EndianLittleBig: // BADCFEHG: bytes Little, word Big : swap bytes
        p[0] = bswap (p[0]);
        p[1] = bswap (p[1]);
        p[2] = bswap (p[2]);
        p[3] = bswap (p[3]);
        break;
      case EndianLittleLittle: // HGFEDCBA: bytes Little, word Little: swap all
        v = bswap (v);
        break;
    }
    d->ptr[0] = p[0];
    d->ptr[1] = p[1];
    d->ptr[2] = p[2];
    d->ptr[3] = p[3];
    if (d->type == Void) {
      d->type = LongLongWord;
    }
  }

  // ---------------------------------------------------------------------------
  void Data::set (char v) {

    set ( (uint8_t) v);
  }

  // ---------------------------------------------------------------------------
  void Data::set (int v) {

    if (sizeof (int) == 2) {

      set ( (uint16_t) v);
    }
    else {

      set ( (uint32_t) v);
    }
  }

  // ---------------------------------------------------------------------------
  void Data::set (long v) {

    if (sizeof (long) == 4) {

      set ( (uint32_t) v);
    }
    else {

      set ( (uint64_t) v);
    }
  }

  // ---------------------------------------------------------------------------
  void Data::set (long long v) {

    set ( (uint64_t) v);
  }

  // ---------------------------------------------------------------------------
  void Data::set (float f) {
    uint32_t v;

    memcpy (&v, &f, sizeof (float));
    set (v);
  }

  // ---------------------------------------------------------------------------
  void Data::set (double d) {
    uint64_t v;

    memcpy (&v, &d, sizeof (double));
    set (v);
  }

  // ---------------------------------------------------------------------------
  void Data::get (uint8_t & uc) const {
    PIMP_D (const Data);
    uint16_t v;
    
    get (v);
    uc = v & 0xFF;
  }

  // ---------------------------------------------------------------------------
  void Data::get (uint16_t & v) const {
    PIMP_D (const Data);

    switch (d->endianness) {

      case EndianLittleBig: // BA
      case EndianLittleLittle: // BA
        v = bswap (d->ptr[0]);
        break;

      default:
        v = d->ptr[0];
        break;
    }

    ntoh (v);
  }

  // ---------------------------------------------------------------------------
  void Data::get (uint32_t & v) const {
    PIMP_D (const Data);

    uint16_t * p = (uint16_t *) &v;

    switch (d->endianness) { // net value: ABCD
      case EndianBigBig: // ABCD: bytes Big, word Big : no swap
        p[0] = d->ptr[0];
        p[1] = d->ptr[1];
        break;
      case EndianBigLittle: // CDAB: bytes Big, word Little : swap words
        p[0] = d->ptr[1];
        p[1] = d->ptr[0];
        break;
      case EndianLittleBig: // BADC: bytes Little, word Big : swap bytes
        p[0] = bswap (d->ptr[0]);
        p[1] = bswap (d->ptr[1]);
        break;
      case EndianLittleLittle: // DCBA: bytes Little, word Little: swap all
        p[0] = d->ptr[0];
        p[1] = d->ptr[1];
        v = bswap (v);
        break;
    }

    ntoh (v);
  }

  // ---------------------------------------------------------------------------
  void Data::get (uint64_t & v) const {
    PIMP_D (const Data);

    uint16_t * p = (uint16_t *) &v;

    switch (d->endianness) { // net value: ABCDEFGH
      case EndianBigBig: // ABCDEFGH: bytes Big, word Big : no swap
        p[0] = d->ptr[0];
        p[1] = d->ptr[1];
        p[2] = d->ptr[2];
        p[3] = d->ptr[3];
        break;
      case EndianBigLittle: // GHEFCDAB: bytes Big, word Little : swap words
        p[0] = d->ptr[3];
        p[1] = d->ptr[2];
        p[2] = d->ptr[1];
        p[3] = d->ptr[0];
        break;
      case EndianLittleBig: // BADCFEHG: bytes Little, word Big : swap bytes
        p[0] = bswap (d->ptr[0]);
        p[1] = bswap (d->ptr[1]);
        p[2] = bswap (d->ptr[2]);
        p[3] = bswap (d->ptr[3]);
        break;
      case EndianLittleLittle: // HGFEDCBA: bytes Little, word Little: swap all
        p[0] = d->ptr[0];
        p[1] = d->ptr[1];
        p[2] = d->ptr[2];
        p[3] = d->ptr[3];
        v = bswap (v);
        break;
    }
    ntoh (v);
  }

  // ---------------------------------------------------------------------------
  void Data::get (char & v) const {

    get ( (uint8_t &) v);
  }

  // ---------------------------------------------------------------------------
  void Data::get (int & v) const {

    if (sizeof (int) == 2) {

      get ( (uint16_t &) v);
    }
    else {

      get ( (uint32_t &) v);
    }
  }

  // ---------------------------------------------------------------------------
  void Data::get (long & v) const {

    if (sizeof (long) == 4) {

      get ( (uint32_t &) v);
    }
    else {

      get ( (uint64_t &) v);
    }
  }

  // ---------------------------------------------------------------------------
  void Data::get (long long & v) const {

    get ( (uint64_t &) v);
  }

  // ---------------------------------------------------------------------------
  void Data::get (float & f) const {
    uint32_t v;

    get (v);
    memcpy (&f, &v, sizeof (float));
  }

  // ---------------------------------------------------------------------------
  void Data::get (double & d) const {
    uint64_t v;

    get (v);
    memcpy (&d, &v, sizeof (double));
  }

  // ---------------------------------------------------------------------------
  Data::Type Data::type() const {
    PIMP_D (const Data);

    return d->type;
  }

  // ---------------------------------------------------------------------------
  void Data::setType (Type t) {
    PIMP_D (Data);

    d->type = t;
  }

  // ---------------------------------------------------------------------------
  size_t Data::size() const {

    if (type() != Void) {
      static const size_t s[] = {2, 2, 4, 8, 4, 8};

      return s[type()];
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  //
  //                         Data::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Data::Private::Private (Data * q, Endian e, Type t) :
    q_ptr (q), endianness (e), type (t) {

    memset (ptr, 0, sizeof (ptr));
  }
}
