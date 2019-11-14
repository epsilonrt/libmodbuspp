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
#pragma once

#ifndef __DOXYGEN__

#include <algorithm>  // std::swap until C++11
#include <utility>    // std::swap since C++11

/*
 * reverse the order of the bytes
 */
template <typename T>
T swapBytes (T input) { // swap bytes
  uint8_t * ptr = reinterpret_cast<uint8_t*> (&input);

  for (std::size_t i = 0; i < sizeof (T) / 2; ++i) {

    std::swap (ptr[i], ptr[ sizeof (T) - 1 - i ]);
  }
  return input;
}

/*
 * reverse the order of the 16-bit words
 */
template <typename T>
T swapWords (T input) { // swap words
  uint16_t * ptr = reinterpret_cast<uint16_t*> (&input);

  for (std::size_t i = 0; i < sizeof (T) / 4; ++i) {

    std::swap (ptr[i], ptr[ (sizeof (T) / 2) - 1 - i ]);
  }
  return input;
}

/*
 * reverse the byte order of each 16-bit word.
 */
template <typename T>
T swapBytesInWords (T input) { // swap bytes in each words
  uint16_t * ptr = reinterpret_cast<uint16_t*> (&input);

  for (std::size_t i = 0; i < sizeof (T) / 2; ++i) {

    ptr[i] = swapBytes (ptr[i]);
  }
  return input;
}

/*
 * converts the input from host byte order to network byte order. 
 */
template <typename T>
T hton (T input) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  return swapBytes (input);
#else
  return input;
#endif
}

/*
 * converts the input from host byte network to host byte order. 
 */
template <typename T>
T ntoh (T input) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  return swapBytes (input);
#else
  return input;
#endif
}
#endif /* __DOXYGEN__ not defined */
/* ========================================================================== */
