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

#include <cstring>    // memcpy ...
#include <modbuspp/global.h>

namespace Modbus {

  /**
   * @class SlaveReport
   * @brief For storing and manipulate server identifier datas returns by the MODBUS 17 function
   */
  template <typename T, Endian e = EndianBig>
  class SlaveReport {

  public:
      /**
       * @brief Default constructor
       */
      SlaveReport() : m_endian (e), m_size (0) {}
      
      SlaveReport (int len, const uint8_t * reply) : SlaveReport() {
        memcpy(m_data, reply, len);
        m_size = len;
      }

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
       * @brief Server ID
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
        uint16_t len = m_size - (sizeof (T) + 1);

        if (len > 0) {

          d.assign (reinterpret_cast<const char *> (&m_data[sizeof (T) + 1]), len);
        }
        return d;
      }
      
      /**
       * @brief number of bytes stored in the underlying array.
       */
      inline uint16_t size() const {

        return m_size;
      }

      friend class Slave;

    protected:
      Endian m_endian;
      uint16_t m_size;
      uint8_t m_data[MODBUS_MAX_PDU_LENGTH];
  };
}
/* ========================================================================== */
