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

#include <modbuspp/slave.h>
#include <modbuspp/request.h>

namespace Modbus {

  class Request;

  /**
    * @class BufferedSlave
    * @brief Buffered Slave
    *
    * @example server/clock-server/main.cpp
    * @example server/tcp-gateway/main.cpp
    *
    * @author Pascal JEAN, aka epsilonrt
    * @copyright GNU Lesser General Public License
    */
  class BufferedSlave : public Slave {

    public:

      friend class Server;

      /**
       * @brief Constructor
       *
       * Constructor of a new buffered slave with the slaveAddr identifier.
       * If the device @b dev is provided, usually of the class @b Master and
       * @b dev->isOpen() returns true:
       * - The requested data is actually read, then stored in the memory
       * buffer before being returned.
       * - The data provided is actually written, after being stored in
       * the memory buffer.
       */
      BufferedSlave (int slaveAddr, Device * dev = 0);

      /**
       * @brief Default Constructor
       *
       * object cannot be used without calling setNumber()
       */
      BufferedSlave ();

      /**
       * @brief Destructor
       */
      virtual ~BufferedSlave();

      /**
       * @brief returns true if number() is sets
       */
      bool isValid() const;

      /**
       * @brief Setting a block of data in the memory buffer.
       *
       * A single block of type @b t can be defined for a given slave.
       * The block has nmeb elements and starts at startAddr.
       * @param t
       * @param nmemb
       * @param startAddr
       * @return
       */
      int setBlock (Table t, int nmemb, int startAddr = -1);

      /**
       * @brief
       * @param cb
       */
      void setBeforeReplyCallback (Message::Callback cb);

      /**
       * @brief
       * @param cb
       */
      void setAfterReplyCallback (Message::Callback cb);

      /**
       * @brief
       * @return
       */
      Message::Callback beforeReplyCallback() const;

      /**
       * @brief
       * @return
       */
      Message::Callback afterReplyCallback() const;

      /**
       * @overload
       */
      virtual int readDiscreteInputs (int addr, bool * dest, int nb = 1);

      /**
       * @overload
       */
      virtual int readCoils (int addr, bool * dest, int nb = 1);

      /**
       * @overload
       */
      virtual int writeCoil (int addr, bool src);

      /**
       * @overload
       */
      virtual int writeCoils (int addr, const bool * src, int nb);

      /**
       * @overload
       */
      virtual int readInputRegisters (int addr, uint16_t * dest, int nb = 1);

      /**
       * @overload
       */
      virtual int readRegisters (int addr, uint16_t * dest, int nb = 1);

      /**
       * @overload
       */
      virtual int writeRegister (int addr, uint16_t value);

      /**
       * @overload
       */
      virtual int writeRegisters (int addr, const uint16_t * src, int nb);

      /**
       * @overload
       */
      virtual int writeReadRegisters (int write_addr, const uint16_t * src, int write_nb,
                                      int  read_addr, uint16_t * dest, int read_nb);

      /**
       * @overload
       */
      virtual int reportSlaveId (uint16_t max_dest, uint8_t * dest);

      /**
       * @brief
       * @param addr
       * @param src
       * @param nb
       * @return
       */
      int writeInputRegisters (int addr, const uint16_t * src, int nb);

      /**
       * @brief
       * @param addr
       * @param src
       * @param nb
       * @return
       */
      int writeDiscreteInputs (int addr, const bool * src, int nb);

      /**
       * @brief
       * @param addr
       * @param src
       * @return
       */
      int writeDiscreteInput (int addr, bool src);

      /**
       * @brief
       * @param addr
       * @param value
       * @return
       */
      int writeInputRegister (int addr, uint16_t value);


      /**
       * @brief Write many input data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall write the content of the @b nb input data
       * from the array @b src at address @b addr of the device.
       *
       * @return number of written input Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1.
       */
      template <typename T, Endian e> int writeInputRegisters (int addr, Data<T, e> * src, int nb = 1) {
        std::vector<uint16_t> buf;

        for (int i = 0; i < nb; i++) {

          src[i].updateRegisters();
          for (auto & r : src[i].registers()) {
            buf.push_back (r);
          }
        }
        return writeInputRegisters (addr, buf.data(), buf.size());;
      }

      /**
       * @brief Write a single input data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall write a single input data
       * from @b value at address @b addr of the device.
       *
       * @return number of written input Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1.
       */
      template <typename T, Endian e> int writeInputRegister (int addr, Data<T, e> & value) {

        value.updateRegisters();
        return writeInputRegisters (addr, value.registers().data(), value.registers().size());
      }

      using Slave::readInputRegisters;
      using Slave::readInputRegister;
      using Slave::readRegister;
      using Slave::readRegisters;
      using Slave::writeRegisters;
      using Slave::writeRegister;
      using Slave::reportSlaveId;

    protected:
      class Private;
      BufferedSlave (Private &dd);
      modbus_mapping_t * map();
      const modbus_mapping_t * map() const;
      int readFromDevice (const Request * req);
      int readFromDevice (const Request & req);
      int writeToDevice (const Request * req);
      int writeToDevice (const Request & req);

    private:
      PIMP_DECLARE_PRIVATE (BufferedSlave)
  };
}

/* ========================================================================== */
