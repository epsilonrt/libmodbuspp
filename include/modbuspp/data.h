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

#include <cstdio>     // printf
#include <cstring>    // memcpy ...
#include <array>
#include <type_traits>
#include <modbuspp/swap.h>

namespace Modbus {

  /**
   * @enum Endian
   * @brief Sequential order in which bytes are arranged 
   */
  enum Endian { 
    EndianBigBig = 0x00,    ///< Bytes in big endian order, word in big endian order : ABCD
    EndianBig = EndianBigBig, ///< Big endian order : ABCD
    EndianBigLittle = 0x01, ///< Bytes in big endian order, word in little endian order : CDAB
    EndianLittleBig = 0x02, ///< Bytes in little endian order, word in big endian order : BADC
    EndianLittleLittle = 0x03, ///< Bytes in little endian order, word in little endian order : DCBA
    EndianLittle = EndianLittleLittle ///< Little endian order : DCBA
  };

  class Device;
  class Master;
  class Slave;

  /**
   * @class Data
   * @brief Arithmetic data in multiple 16-bit Modbus registers
   * 
   * Data is a template class for storing, transmitting, and receiving 
   * arithmetic data in multiple 16-bit Modbus registers.
   * 
   * @param T is a type of arithmetic data (int, float ...) of a size greater 
   * than or equal to 2.
   * @param e is the order of bytes and words in the data model used by the 
   * user's Modbus network. By default it is the big endian order for bytes 
   * and words that is used.
   * 
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  template <typename T, Endian e = EndianBig>
  class Data {
    public:
      static_assert ( (sizeof (T) >= 2 && (sizeof (T) % 2) == 0), "Bad Data typename !");
      static_assert (std::is_arithmetic<T>::value, "Arithmetic type required !");

      /**
       * @brief Default constructor
       * 
       * The default value of T is 0.
       */
      Data() : m_endian (e) {
        m_value = 0;
        updateRegisters();
      }

      /**
       * @brief Constructor from a value of T
       */
      Data (const T& t) :  m_value (t), m_endian (e) {
        updateRegisters();
      }

      /**
       * @brief  Overload of the reference operator on the T value
       */
      operator T&() {
        return m_value;
      }

      /**
       * @brief  Overload of the reference operator on the T value
       */
      operator T&() const {
        return m_value;
      }

      /**
       * @brief Access to the T value
       */
      T& value() {
        return m_value;
      }

      /**
       * @brief Access to the T value
       */
      const T& value() const {
        return m_value;
      }

      /**
       * @brief Overload of the pointer operator on the T value
       */
      T* operator&() {
        return & m_value;
      }

      /**
       * @brief Overload of the pointer operator on the T value
       */
      const T* operator&() const {
        return & m_value;
      }

      /**
       * @brief Overload of the assignment operator from a T value
       */
      T& operator= (const T& t) {
        m_value = t;
        updateRegisters();
        return m_value;
      }

      /**
       * @brief Return the bytes and words endianness
       */
      Endian endianness() const {
        return m_endian;
      }

      /**
       * @brief Number of bytes of type T
       */
      std::size_t size() const {
        return sizeof (m_value);
      }

      /**
       * @brief Array of Modbus registers corresponding to the T value
       */
      std::array < uint16_t, sizeof (T) / 2 > & registers() {
        return m_registers;
      }
      
      /**
       * @brief Array of Modbus registers corresponding to the T value
       */
      const std::array < uint16_t, sizeof (T) / 2 > & registers() const {
        return m_registers;
      }

      /**
       * @brief Swap bytes and words of a T value \b v
       * 
       * The order used is \b endianness().
       */
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

      /**
       * @brief Prints the hexadecimal values of a byte array
       * 
       * For debugging purpose.
       */
      static void print (const uint8_t * p, const size_t s) {
        std::printf ("0x");
        for (std::size_t i = 0; i < s; i++) {
          std::printf ("%02X", p[i]);
        }
        std::printf ("\n");
      }

      /**
       * @brief Prints the hexadecimal values of T value
       * 
       * For debugging purpose.
       */
      static void print (const T & v) {
        print ( (const uint8_t *) &v, sizeof (v));
      }

      /**
       * @brief Prints the hexadecimal values of the current T value
       * 
       * For debugging purpose.
       */
      void print () {
        updateRegisters();
        print ( (const uint8_t *) m_registers.data(), size());
      }

      friend class Device;
      friend class Master;
      friend class Slave;

  protected:

#ifndef __DOXYGEN__
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
#endif /* __DOXYGEN__ not defined */

    private:
      T m_value;
      Endian m_endian;
      std::array < uint16_t, sizeof (T) / 2 > m_registers;
  };

  /**
   * @class SlaveId
   * @brief For storing and manipulate slave identifier datas returns by the MODBUS 17 function
   */
  template <typename T, Endian e = EndianBig>
  class SlaveId {

  public:
      /**
       * @brief Default constructor
       */
      SlaveId() : m_endian (e), m_size (0) {}

      /**
       * @brief Run Indicator Status
       * @return true if ON.
       */
      bool status() const {
        
        if (m_size > sizeof (T)) {
          
          return m_data[sizeof (T)] == 0xFF;
        }
        return false;
      }

      /**
       * @brief Slave ID
       * @return the returned value is of type T which depends on the device
       */
      T id() const {
        T v = 0;

        if (m_size >= sizeof (T)) {

          std::memcpy (&v, m_data, sizeof (T));

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
          return ntoh (v);
        }

        return v;
      }

      /**
       * @brief Additional Data
       * @return 
       */
      std::string data() const {
        std::string d;
        size_t len = m_size - (sizeof (T) + 1);

        if (len > 0) {

          d.assign (reinterpret_cast<const char *> (&m_data[sizeof (T) + 1]), len);
        }
        return d;
      }
      
      /**
       * @brief number of bytes stored in the underlying array.
       */
      inline size_t size() const {

        return m_size;
      }

      friend class Master;

    protected:
      Endian m_endian;
      size_t m_size;
      uint8_t m_data[MODBUS_MAX_PDU_LENGTH];
  };
}
/* ========================================================================== */
