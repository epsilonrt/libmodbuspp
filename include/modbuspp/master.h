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

//#include <iostream>
#include <modbuspp/device.h>

namespace Modbus {
  
  /**
   * @class Master
   * @brief Master connected to Modbus (Client)
   *
   * The Modbus master is the only one able to initiate a transaction with
   * the slaves. This class therefore makes it possible to read or write in
   * Modbus slaves.
   * 
   * To use, simply perform the following actions:
   * @code
      // instantiate a variable by choosing the network and the parameters to connect to it
      Master mb (Rtu, port , "38400E1");
      // open the communication
      mb.open ();
      // if necessary, choose the slave, eg:
      mb.setSlave (33);
      // perform read or write operations of slaves
      mb.readInputRegisters (1, values, 2);
   * @endcode
   *
   * @example master/read-holding-data/main.cpp
   * @example master/read-input-registers/main.cpp
   * @example master/write-holding-data/main.cpp
   * @example master/read-coils/main.cpp
   * 
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Master : public Device {

    public:
      /**
       * @brief Constructor
       *
       * Constructs a Modbus master for the \b net network.
       *
       * For the Tcp backend :
       * - \b connection specifies the host name or IP
       * address of the host to connect to, eg. "192.168.0.5" , "::1" or
       * "server.com". A NULL value can be used to listen any addresses in server mode,
       * - \b settings is the service name/port number to connect to.
       * To use the default Modbus port use the string "502". On many Unix
       * systems, it’s convenient to use a port number greater than or equal
       * to 1024 because it’s not necessary to have administrator privileges.
       * .
       *
       * For the Rtu backend :
       * - \b connection specifies the name of the serial port handled by the
       *  OS, eg. "/dev/ttyS0" or "/dev/ttyUSB0",
       * - \b settings specifies communication settings as a string in the
       *  format BBBBPS. BBBB specifies the baud rate of the communication, PS
       *  specifies the parity and the bits of stop. \n
       *  According to Modbus RTU specifications :
       *    - the possible combinations for PS are E1, O1 and N2.
       *    - the number of bits of data must be 8, also there is no possibility
       *      to change this setting
       *    .
       * .
       *
       * An exception std::invalid_argument is thrown if one of the parameters
       * is incorrect.
       */
      Master (Net net = Tcp, const std::string & connection = "*",
              const std::string & settings = "502");
      /**
       * @brief Destructor
       *
       * The destructor closes the connection if it is open and releases all
       * affected resources.
       */
      virtual ~Master();

      using Device::readInputRegisters;
      using Device::readInputRegister;
      using Device::readRegister;
      using Device::readRegisters;
      using Device::writeRegisters;
      using Device::writeRegister;

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
      int readDiscreteInputs (int addr, bool * dest, int nb = 1);

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
      int readCoils (int addr, bool * dest, int nb = 1);

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
      int writeCoil (int addr, bool src);

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
      int writeCoils (int addr, const bool * src, int nb);

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
      int readInputRegisters (int addr, uint16_t * dest, int nb = 1);

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
      int readRegisters (int addr, uint16_t * dest, int nb = 1);

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
      int writeRegister (int addr, uint16_t value);

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
      int writeRegisters (int addr, const uint16_t * src, int nb);

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
      int writeReadRegisters (int write_addr, const uint16_t * src, int write_nb,
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
      int reportSlaveId (int max_dest, uint8_t * dest);
      
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
      Master (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Master)
  };
}

/* ========================================================================== */
