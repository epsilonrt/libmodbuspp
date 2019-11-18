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

#include <string>
#include <vector>
#include <modbuspp/data.h>

namespace Modbus {

  /**
   * @class DataModel
   * @brief DataModel for Modbus
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class DataModel  {
    public:

      /**
       * @brief Constructor
       */
      DataModel (int slaveAddr = TcpSlave);

      /**
       * @brief Destructor
       */
      virtual ~DataModel();

      /**
       * @brief Get slave number
       *
       * This function shall get the slave number.
       *
       * @return the slave number
       */
      int slave() const;

      /**
       * @brief Modbus addressing mode
       *
       * This function shall return the Modbus addressing mode used.
       * The address mode used is, by default, that of the data model, that is
       * to say, a numbering of the registers from 1 to n.
       *
       * The Modbus application protocol defines precisely PDU addressing rules.
       * In a Modbus PDU each data is addressed from 0 to 65535.
       *
       * It also defines clearly a Modbus data model composed of 4 blocks that
       * comprises several elements numbered from 1 to n.
       *
       * In the Modbus data Model each element within a data block is numbered
       * from 1 to n.
       *
       * Afterwards the Modbus data model has to be bound to the device
       * application (IEC -61131 object, or other application model).
       *
       * @return true for Modbus PDU adressing
       * @sa setPduAddressing()
       */
      bool pduAddressing() const;

      /**
       * @brief Set the Modbus addressing mode
       *
       * @param pduAddressing true for Modbus PDU adressing
       * @sa pduAddressing()
       */
      void setPduAddressing (bool pduAddressing = true);

      /**
       * @brief returns the PDU address corresponding to an address in the MODBUS data model.
       *
       * If the PDU addressing mode is not enabled (which is the case by default),
       * the PDU address is equal to the MODBUS address minus 1, otherwise they are equal.
       */
      int pduAddress (int addr) const;

      /**
       * @brief Read many discrete inputs (input bits)
       *
       * This function shall read the content of the \b nb input bits to the
       * address \b addr of the device.  The result of reading is stored
       * in \b dest array as  boolean.
       *
       * The function uses the Modbus function code 0x02 (read input status).
       *
       * @return the number of read input status if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readDiscreteInputs (int addr, bool * dest, int nb = 1) = 0;

      /**
       * @brief Read many coils (bits)
       *
       * This function shall read the status of the \b nb bits (coils) to the
       * address \b addr of the device. The result of reading is stored
       * in \b dest array as boolean.
       *
       * The function uses the Modbus function code 0x01 (read coil status).
       *
       * @return the number of read bits if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readCoils (int addr, bool * dest, int nb = 1) = 0;

      /**
       * @brief Write a single coil (bit)
       *
       * This function shall write the status of src at the address addr of
       * the device.
       *
       * The function uses the Modbus function code 0x05 (force single coil).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeCoil (int addr, bool src) = 0;

      /**
       * @brief Write many coils (bits)
       *
       * This function shall write the status of the \b nb bits (coils) from
       * \b src at the address \b addr of the device.
       * The \b src array must contains booelans.
       *
       * The function uses the Modbus function code 0x0F (force multiple coils).
       *
       * @return the number of written bits if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeCoils (int addr, const bool * src, int nb) = 0;

      /**
       * @brief Read many input registers
       *
       * This function shall read the content of the \b nb input registers to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest array as word values (16 bits).
       *
       * The function uses the Modbus function code 0x04 (read input registers).
       * The holding registers and input registers have different historical
       * meaning, but nowadays it's more common to use holding registers only.
       *
       * @return return the number of read input registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readInputRegisters (int addr, uint16_t * dest, int nb = 1) = 0;

      /**
       * @brief Read many registers
       *
       * This function shall read the content of the nb holding registers to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest array as word values (16 bits).
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return return the number of read registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readRegisters (int addr, uint16_t * dest, int nb = 1) = 0;

      /**
       * @brief Write a single register
       *
       * This function shall write the value of \b value holding registers at
       * the address \b addr of the device.
       *
       * The function uses the Modbus function code 0x06 (preset single register).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeRegister (int addr, uint16_t value) = 0;

      /**
       * @brief Write many registers
       *
       * This function shall write the content of the \b nb holding registers
       * from the array \b src at address \b addr of the device.
       *
       * The function uses the Modbus function code 0x10 (preset multiple registers).
       *
       * @return number of written registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeRegisters (int addr, const uint16_t * src, int nb) = 0;

      /**
       * @brief Read a single discrete input (input bit)
       *
       * This function shall read a single input bits to the address \b addr
       * of the device.  The result of reading is stored in \b dest
       * as boolean.
       *
       * The function uses the Modbus function code 0x02 (read input status).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      inline int readDiscreteInput (int addr, bool & dest) {

        return readDiscreteInputs (addr, &dest, 1);
      }

      /**
       * @brief Read a single coil (bit)
       *
       * This function shall read a signle bit (coil) to the address \b addr of
       * the device. The result of reading is stored in \b dest
       * as boolean.
       *
       * The function uses the Modbus function code 0x01 (read coil status).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      inline int readCoil (int addr, bool & dest) {

        return readCoils (addr, &dest, 1);
      }

      /**
       * @brief Read a single input register
       *
       * This function shall read a single input register to the address \b addr
       * of the device.
       *
       * The result of reading is stored in \b dest as word values (16 bits).
       *
       * The function uses the Modbus function code 0x04 (read input registers).
       * The holding registers and input registers have different historical
       * meaning, but nowadays it's more common to use holding registers only.
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      inline int readInputRegister (int addr, uint16_t & dest) {

        return readInputRegisters (addr, &dest, 1);
      }

      /**
       * @brief Read a single register
       *
       * This function shall read a signle holding register to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest as word values (16 bits).
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      inline int readRegister (int addr, uint16_t & dest) {

        return readRegisters (addr, &dest, 1);
      }

      /**
       * @brief Read many input data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall read the content of the \b nb input data to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest array as \b T values.
       *
       * The function uses the Modbus function code 0x04 (read input registers).
       *
       * @return return the number of read input Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int readInputRegisters (int addr, Data<T, e> * dest, int nb = 1) {
        int ret;
        std::vector<uint16_t> buf (nb * dest[0].registers().size(), 0);

        ret = readInputRegisters (addr, buf.data(), buf.size());
        if (static_cast<std::size_t> (ret) == buf.size()) {
          int n = 0;

          for (int i = 0; i < nb; i++) {

            for (auto & r : dest[i].registers()) {

              r = buf[n++];
            }
            dest[i].updateValue();
          }
        }
        return ret;
      }

      /**
       * @brief Read a single input data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall read a single input data to the address \b addr
       * of the device.
       *
       * The result of reading is stored in \b dest as T value.
       *
       * The function uses the Modbus function code 0x04 (read input registers).
       *
       * @return return the number of read input Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int readInputRegister (int addr, Data<T, e> & dest) {
        int s = dest.registers().size();

        int r = readInputRegisters (addr, dest.registers().data(), s);
        if (r == s) {

          dest.updateValue();
        }
        return r;
      }

      /**
       * @brief Read a single holding data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall read a single holding data to the address \b addr
       * of the device.
       *
       * The result of reading is stored in \b dest as T value.
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return return the number of read holding Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int readRegister (int addr, Data<T, e> & dest) {
        int s = dest.registers().size();

        int r = readRegisters (addr, dest.registers().data(), s);
        if (r == s) {

          dest.updateValue();
        }
        return r;
      }

      /**
       * @brief Read many holding data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall read the content of the \b nb data to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest array as \b T values.
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return return the number of read holding Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int readRegisters (int addr, Data<T, e> * dest, int nb = 1) {
        int ret;
        std::vector<uint16_t> buf (nb * dest[0].registers().size(), 0);

        ret = readRegisters (addr, buf.data(), buf.size());
        if (static_cast<std::size_t> (ret) == buf.size()) {
          int n = 0;

          for (int i = 0; i < nb; i++) {

            for (auto & r : dest[i].registers()) {

              r = buf[n++];
            }
            dest[i].updateValue();
          }
        }
        return ret;
      }

      /**
       * @brief Write many holding data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall write the content of the \b nb holding data
       * from the array \b src at address \b addr of the device.
       *
       * The function uses the Modbus function code 0x10 (preset multiple registers).
       *
       * @return number of written holding Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int writeRegisters (int addr, const Data<T, e> * src, int nb = 1) {
        std::vector<uint16_t> buf;

        for (int i = 0; i < nb; i++) {
          for (auto & r : src[i].registers()) {
            buf.push_back (r);
          }
        }
        return writeRegisters (addr, buf.data(), buf.size());;
      }

      /**
       * @brief Write a single holding data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall write a single holding data
       * from \b value at address \b addr of the device.
       *
       * The function uses the Modbus function code 0x10 (preset multiple registers).
       *
       * @return number of written holding Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int writeRegister (int addr, const Data<T, e> & value) {

        return writeRegisters (addr, value.registers().data(), value.registers().size());
      }
      
      /**
       * @brief  Set many booleans from an array of bytes 
       * 
       * All the bits of the bytes read from the first position of the array src
       * are written as booleans in the dest array.
       * @param dest 
       * @param src
       * @param n
       */
      static void setBoolArray (bool * dest, const uint8_t * src, size_t n);
      
    protected:
      class Private;
      DataModel (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (DataModel)
  };
}

/* ========================================================================== */
