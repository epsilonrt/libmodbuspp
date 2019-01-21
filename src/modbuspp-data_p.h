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

#ifndef MODBUSPP_DATA_PRIVATE_H
#define MODBUSPP_DATA_PRIVATE_H

#include <cerrno>
#include <algorithm>   // std::swap until C++11
#include <utility>     // std::swap since C++11
#include <modbuspp-data.h>

namespace Modbus {

  template <typename T>
  T bswap (T input) {
    char * ptr = reinterpret_cast<char*> (&input);

    for (std::size_t i = 0; i < sizeof (T) / 2; ++i) {

      std::swap (ptr[i], ptr[ sizeof (T) - 1 - i ]);
    }
    return input;
  }

  template <typename T>
  void hton (T & input) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
     input = bswap (input);
#endif
  }

  template <typename T>
  void ntoh (T & input) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
     input = bswap (input);
#endif
  }

  class Data::Private {

    public:
      Private (Data * q, Endian e, Type type);
      Data * const q_ptr;
      uint16_t ptr[4];
      Endian endianness;
      Type type;

      PIMP_DECLARE_PUBLIC (Data)
  };

}

/* ========================================================================== */
#endif /* MODBUSPP_DATA_PRIVATE_H defined */
