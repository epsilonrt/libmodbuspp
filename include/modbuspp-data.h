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

#ifndef MODBUSPP_DATA_H
#define MODBUSPP_DATA_H

#include <cstdio>     // printf
#include <cstring>    // mencpy ...
#include <array>
#include <type_traits>
#include "modbuspp-swap.h"

#ifndef __DOXYGEN__
#endif /* __DOXYGEN__ not defined */

/**
 *
 */
namespace Modbus {

  enum Endian { // network number ABCD
    EndianBigBig = 0x00,    // bytes in big endian order, word in big endian order : ABCD
    EndianBig = EndianBigBig, // big endian order : ABCD
    EndianBigLittle = 0x01, // bytes in big endian order, word in little endian order : CDAB
    EndianLittleBig = 0x02, // bytes in little endian order, word in big endian order : BADC
    EndianLittleLittle = 0x03, // bytes in little endian order, word in little endian order : DCBA
    EndianLittle = EndianLittleLittle // little endian order : DCBA
  };


  class Device;
  class Master;

  /**
   * @class Data
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   * @brief 
   */
  template <typename T, Endian e = EndianBigBig>
  class Data {
    public:
      static_assert ( (sizeof (T) >= 2 && (sizeof (T) % 2) == 0), "Bad Data typename !");
      static_assert (std::is_arithmetic<T>::value, "Arithmetic type required.");

      Data() : m_endian (e) {
        m_value = 0;
        updateRegisters();
      }

      Data (const T& t) :  m_value (t), m_endian (e) {
        updateRegisters();
      }

      operator T&() {
        return m_value;
      }

      operator T&() const {
        return m_value;
      }

      T& value() {
        return m_value;
      }

      const T& value() const {
        return m_value;
      }

      T* operator&() {
        return & m_value;
      }

      const T* operator&() const {
        return & m_value;
      }

      T& operator= (const T& t) {
        m_value = t;
        updateRegisters();
        return m_value;
      }

      Endian endianness() const {
        return m_endian;
      }

      std::size_t size() const {
        return sizeof (m_value);
      }

      std::array < uint16_t, sizeof (T) / 2 > & registers() {
        return m_registers;
      }
      
      const std::array < uint16_t, sizeof (T) / 2 > & registers() const {
        return m_registers;
      }


      void swap (T & v) {

        switch (m_endian) { // net value: ABCDEFGH
          case EndianBigBig: // ABCDEFGH: bytes Big, word Big : no swap
            break;
          case EndianBigLittle: // GHEFCDAB: bytes Big, word Little : swap words
            v = swapWords (v);
            break;
          case EndianLittleBig: // BADCFEHG: bytes Little, word Big : swap bytes of words
            v = swapBytesInWords (v);
            break;
          case EndianLittleLittle: // HGFEDCBA: bytes Little, word Little: swap all
            v = swapBytes (v);
            break;
        }
      }

      // debug purpose
      static void print (const uint8_t * p, const size_t s) {
        std::printf ("0x");
        for (std::size_t i = 0; i < s; i++) {
          std::printf ("%02X", p[i]);
        }
        std::printf ("\n");
      }

      static void print (const T & v) {
        print ( (const uint8_t *) &v, sizeof (v));
      }

      void print () {
        updateRegisters();
        print ( (const uint8_t *) m_registers.data(), size());
      }

      friend class Device;
      friend class Master;

    protected:

      // update MODBUS registers from data value
      // to call before writing in the MODBUS registers
      void updateRegisters() {
        T v;

        v = hton (m_value);
        swap (v);
        std::memcpy (m_registers.data(), &v, sizeof (T));
        for (auto & r : m_registers) {
          r = ntoh (r);
        }
      }

      // update data value from MODBUS registers
      // to call after reading the modbus registers
      void updateValue() {
        T v;

        for (auto & r : m_registers) {
          r = hton (r);
        }
        std::memcpy (&v, m_registers.data(), sizeof (T));
        swap (v);
        m_value = ntoh (v);
      }

    private:
      T m_value;
      Endian m_endian;
      std::array < uint16_t, sizeof (T) / 2 > m_registers;
  };
}
/**
 *  @}
 */
/* ========================================================================== */
#endif /* MODBUSPP_DATA_H defined */
