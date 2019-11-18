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

namespace Modbus {
  
  class Message;
  
  /**
    * @class BufferedSlave
    * @brief Buffered Slave
    *
    * @example slave/..../main.cpp
    *
    * @author Pascal JEAN, aka epsilonrt
    * @copyright GNU Lesser General Public License
    */
  class BufferedSlave : public Slave {

    public:

      friend class Server;

      /**
       * @brief Constructor
       */
      BufferedSlave (int slaveAddr, Device * dev = 0);

      /**
       * @brief Destructor
       */
      virtual ~BufferedSlave();

      using Slave::readInputRegisters;
      using Slave::readInputRegister;
      using Slave::readRegister;
      using Slave::readRegisters;
      using Slave::writeRegisters;
      using Slave::writeRegister;
      using Slave::reportSlaveId;

      /**
       * @brief
       * @param t
       * @param nmemb
       * @param startAddr
       * @return
       */
      int setBlock (Table t, int nmemb, int startAddr = 1);
      
      int readFromDevice (const Message * req);
      int readFromDevice (const Message & req);
      int writeToDevice (const Message * req);
      int writeToDevice (const Message & req);
      
      void setBeforeReplyCallback (Message::Callback cb);
      void setAfterReplyCallback (Message::Callback cb);
      Message::Callback beforeReplyCallback() const;
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

    protected:
      class Private;
      BufferedSlave (Private &dd);
      modbus_mapping_t * map();
      const modbus_mapping_t * map() const;

    private:
      PIMP_DECLARE_PRIVATE (BufferedSlave)
  };
}

/* ========================================================================== */
