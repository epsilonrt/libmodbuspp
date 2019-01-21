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

#ifndef MODBUSPP_DATA_H
#define MODBUSPP_DATA_H

#include <string>
#include <modbus.h>
#include "global.h"

/**
 *
 */
namespace Modbus {

  /**
   * @class Data
   * @brief
   */
  class Data {
    public:
      enum Type {
        Byte, // 2 Bytes (nothing is smaller than the word on MODBUS)
        Word, // 2 Bytes - 1 MODBUS register
        LongWord, // 4 Bytes - 2 MODBUS registers
        LongLongWord, // 8 Bytes - 4 MODBUS registers
        Float, // 4 Bytes - 2 MODBUS registers
        Double, // 8 Bytes - 4 MODBUS registers
        Void = -1
      };

      enum Endian { // network number ABCD
        EndianBigBig = 0x00,    // bytes in big endian order, word in big endian order : ABCD
        EndianBig = EndianBigBig, // big endian order : ABCD
        EndianBigLittle = 0x01, // bytes in big endian order, word in little endian order : CDAB
        EndianLittleBig = 0x02, // bytes in little endian order, word in big endian order : BADC
        EndianLittleLittle = 0x03, // bytes in little endian order, word in little endian order : DCBA
        EndianLittle = EndianLittleLittle // little endian order : DCBA
      };

      Data (Type type = Void, Endian endian = EndianBig); // default constructor
      Data (const Data & other);
      virtual ~Data();

      Data (uint8_t v, Endian endian = EndianBig);
      Data (uint16_t v, Endian endian = EndianBig);
      Data (uint32_t v, Endian endian = EndianBig);
      Data (uint64_t v, Endian endian = EndianBig);
      Data (char v, Endian endian = EndianBig);
      Data (int v, Endian endian = EndianBig);
      Data (long v, Endian endian = EndianBig);
      Data (long long v, Endian endian = EndianBig);
      Data (float v, Endian endian = EndianBig);
      Data (double v, Endian endian = EndianBig);

      void swap (Data &other);
      Data& operator= (const Data &other);

      void set (uint8_t v);
      void set (uint16_t v);
      void set (uint32_t v);
      void set (uint64_t v);

      void set (char v);
      void set (int v);
      void set (long v);
      void set (long long v);
      void set (float v);
      void set (double v);

      void get (uint8_t & v) const;
      void get (uint16_t & v) const;
      void get (uint32_t & v) const;
      void get (uint64_t & v) const;

      void get (char & v) const;
      void get (int & v) const;
      void get (long & v) const;
      void get (long long & v) const;
      void get (float & v) const;
      void get (double & v) const;

      Endian endianness() const;
      Type type() const;
      void setType (Type t);
      size_t size() const;
      uint16_t * ptr();
      const uint16_t * ptr() const;

      // debug purpose
      void print () const;
      static void print (const uint8_t * p, const size_t s);
      static void print (const uint8_t & v);
      static void print (const uint16_t & v);
      static void print (const uint32_t & v);
      static void print (const uint64_t & v);
      static void print (const char & v);
      static void print (const int & v);
      static void print (const long & v);
      static void print (const long long & v);
      static void print (const float & v);
      static void print (const double & v);

    protected:
      class Private;
      Data (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Data)
  };
}
/**
 *  @}
 */
/* ========================================================================== */
#endif /* MODBUSPP_DATA_H defined */
