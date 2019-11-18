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

#include <modbuspp/master.h>
#include <modbuspp/message.h>
#include <modbuspp/bufferedslave.h>

namespace Modbus {

 /**
   * @class Server
   * @brief Server connected to Modbus (Server)
   *
   * The Modbus slave is waiting for request from Modbus Masters (clients) and 
   * must answer when it is concerned by the request.
   * 
   * To use, simply perform the following actions:
   * @code
      // instantiate a variable by choosing the network and the parameters to connect to it
      Server mb (Rtu, port , "38400E1");
      // open the communication
      mb.open ();
      // if necessary, choose the slave, eg:
      mb.setSlave (33);
      // perform read or write operations of slaves
      mb.readInputRegisters (1, values, 2);
   * @endcode
   *
   * @example slave/..../main.cpp
   * 
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Server : public Device {

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
      Server (Net net = Tcp, const std::string & connection = "*",
              const std::string & settings = "502");
      /**
       * @brief Destructor
       *
       * The destructor closes the connection if it is open and releases all
       * affected resources.
       */
      virtual ~Server();

      /**
       * @brief Establish a Modbus connection
       *
       * This function shall establish a connection to a Modbus server,
       * a network or a bus
       *
       * @return true if successful.
       * Otherwise it shall return -1 and set errno.
       */
      bool open();

      /**
       * @brief Close a Modbus connection
       */
      void close();
      
      /**
       * @brief 
       * @return 
       */
      int task();

      BufferedSlave & addSlave (int slaveAddr, Master * master = 0);
      
      /**
       * @brief 
       * 
       * The broadcast address is \b Broadcast. This special
       * value must be use when you want all Modbus devices of the network
       * receive the request.
       * 
       * @param slaveAddr
       * @return  the slave by reference
       */
      BufferedSlave & slave (int slaveAddr = -1);
      
      /**
       * @brief 
       * @param slaveAddr
       * @return 
       */
      const BufferedSlave & slave (int slaveAddr = -1) const;
      
      /**
       * @brief 
       * @param slaveAddr
       * @return  the slave by reference
       */
      BufferedSlave * slavePtr (int slaveAddr = -1);
      
      /**
       * @brief 
       * @param slaveAddr
       * @return  the pointer on the slave
       */
      const BufferedSlave * slavePtr (int slaveAddr = -1) const;
      
      /**
       * @brief 
       */
      BufferedSlave &operator[] (int);
      
      /**
       * @brief 
       */
      const BufferedSlave &operator[] (int) const;
      
      /**
       * @brief 
       * @param slaveAddr
       * @return 
       */
      bool hasSlave (int slaveAddr) const;
      
    protected:
      class Private;
      Server (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Server)
  };
}

/* ========================================================================== */
