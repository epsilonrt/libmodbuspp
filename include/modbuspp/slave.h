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

#include <modbuspp/datamodel.h>
#include <modbuspp/slaveid.h>

namespace Modbus {
  class Device;
  
 /**
   * @class Slave
   * @brief Slave connected to Modbus
   *
   * @example slave/..../main.cpp
   * 
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Slave : public DataModel {

    public:
      /**
       * @brief Constructor
       */
      Slave (int slaveAddr, Device * dev);

      /**
       * @brief Destructor
       *
       * The destructor closes the connection if it is open and releases all
       * affected resources.
       */
      virtual ~Slave();

      using DataModel::readInputRegisters;
      using DataModel::readInputRegister;
      using DataModel::readRegister;
      using DataModel::readRegisters;
      using DataModel::writeRegisters;
      using DataModel::writeRegister;

      /**
       * @brief Read many discrete inputs (input bits)
       *
       * This function shall read the content of the \b nb input bits to the
       * address \b addr of the remote device.  The result of reading is stored
       * in \b dest array as  boolean.
       *
       * The function uses the Modbus function code 0x02 (read input status).
       *
       * @return the number of read input status if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readDiscreteInputs (int addr, bool * dest, int nb = 1);

      /**
       * @brief Read many coils (bits)
       *
       * This function shall read the status of the \b nb bits (coils) to the
       * address \b addr of the remote device. The result of reading is stored
       * in \b dest array as boolean.
       *
       * The function uses the Modbus function code 0x01 (read coil status).
       *
       * @return the number of read bits if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readCoils (int addr, bool * dest, int nb = 1);

      /**
       * @brief Write a single coil (bit)
       *
       * This function shall write the status of src at the address addr of
       * the remote device.
       *
       * The function uses the Modbus function code 0x05 (force single coil).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeCoil (int addr, bool src);

      /**
       * @brief Write many coils (bits)
       *
       * This function shall write the status of the \b nb bits (coils) from
       * \b src at the address \b addr of the remote device.
       * The \b src array must contains booelans.
       *
       * The function uses the Modbus function code 0x0F (force multiple coils).
       *
       * @return the number of written bits if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeCoils (int addr, const bool * src, int nb);

      /**
       * @brief Read many input registers
       *
       * This function shall read the content of the \b nb input registers to
       * the address \b addr of the remote device.
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
      virtual int readInputRegisters (int addr, uint16_t * dest, int nb = 1);

      /**
       * @brief Read many registers
       *
       * This function shall read the content of the nb holding registers to
       * the address \b addr of the remote device.
       *
       * The result of reading is stored in \b dest array as word values (16 bits).
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return return the number of read registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readRegisters (int addr, uint16_t * dest, int nb = 1);

      /**
       * @brief Write a single register
       *
       * This function shall write the value of \b value holding registers at
       * the address \b addr of the remote device.
       *
       * The function uses the Modbus function code 0x06 (preset single register).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeRegister (int addr, uint16_t value);

      /**
       * @brief Write many registers
       *
       * This function shall write the content of the \b nb holding registers
       * from the array \b src at address \b addr of the remote device.
       *
       * The function uses the Modbus function code 0x10 (preset multiple registers).
       *
       * @return number of written registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeRegisters (int addr, const uint16_t * src, int nb);

      /**
       * @brief Write and read many registers in a single transaction
       *
       * This function shall write the content of the \b write_nb holding
       * registers from the array 'src' to the address \b write_addr of the
       * remote device then shall read the content of the \b read_nb holding
       * registers to the address \b read_addr of the remote device.
       * The result of reading is stored in \b dest array as word values (16 bits).
       *
       * You must take care to allocate enough memory to store the results in
       * \b dest (at least \b nb * sizeof(uint16_t)).
       *
       * The function uses the Modbus function code 0x17 (write/read registers).
       *
       * @return the number of read registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeReadRegisters (int write_addr, const uint16_t * src, int write_nb,
                              int  read_addr, uint16_t * dest, int read_nb);

      /**
       * @brief returns a description of the controller
       *
       * This function shall send a request to the controller to obtain a
       * description of the controller. \n
       * The response stored in dest contains:
       * - the slave ID, this unique ID is in reality not unique at all so it’s
       * not possible to depend on it to know how the information are packed in
       * the response.
       * - the run indicator status (0x00 = OFF, 0xFF = ON)
       * - additional data specific to each controller. For example,
       * libmodbuspp returns the version of the library as a string.
       * .
       * The function writes at most max_dest bytes from the response to dest
       * so you must ensure that dest is large enough.
       * @return return the number of read data if successful.
       * If the output was truncated due to the max_dest limit then the return
       * value is the number of bytes which would have been written to dest if
       * enough space had been available. Thus, a return value greater than
       * max_dest means that the response data was truncated.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int reportSlaveId (uint16_t max_dest, uint8_t * dest);

      /**
       * @brief returns a description of the controller
       *
       * SlaveId is a template class for storing and manipulate slave identifier
       * datas returns by the MODBUS 17 function.
       * @param dest description of the controller
       * @return return the number of read data bytes if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e = EndianBig> int reportSlaveId (SlaveId<T, e> & dest) {
        int r = reportSlaveId (MODBUS_MAX_PDU_LENGTH, dest.m_data);
        if (r >= 0) {
          dest.m_size = r;
        }
        return r;
      }

#ifndef __DOXYGEN__
      template <Endian e = EndianBig> int reportSlaveId (SlaveId<uint8_t, e> & dest) {
        int r = reportSlaveId (MODBUS_MAX_PDU_LENGTH, dest.m_data);
        if (r >= 0) {
          dest.m_size = r;
        }
        return r;
      }

      template <Endian e = EndianBig> int reportSlaveId (SlaveId<uint16_t, e> & dest) {
        int r = reportSlaveId (MODBUS_MAX_PDU_LENGTH, dest.m_data);
        if (r >= 0) {
          dest.m_size = r;
        }
        return r;
      }

      template <Endian e = EndianBig> int reportSlaveId (SlaveId<uint32_t, e> & dest) {
        int r = reportSlaveId (MODBUS_MAX_PDU_LENGTH, dest.m_data);
        if (r >= 0) {
          dest.m_size = r;
        }
        return r;
      }
#endif /* __DOXYGEN__ not defined */

    protected:
      class Private;
      Slave (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Slave)

  };
}

/* ========================================================================== */
