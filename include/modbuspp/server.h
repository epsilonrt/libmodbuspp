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
    * 
    * @code
       // instantiate a variable by choosing the network and the parameters to connect to it
       Server srv (net, connection, settings);

       // Adding a new slave to the server
       BufferedSlave & slv = srv.addSlave (10);

       // Adding Input registers block to this slave
       slv.setBlock (InputRegister, 8);

       // open the communication
       srv.open ();
       for (;;) {
         now = time (nullptr);
         if (now > before) {
           before = now;
           // .....
           // update the input registers
           slv.writeInputRegisters (1, mb_time, 8);
         }
         srv.poll (100);
       }
    * @endcode
    *
     * @example server/clock-server/main.cpp
     * @example server/tcp-gateway/main.cpp
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
      virtual bool open();

      /**
       * @brief Close a Modbus connection
       */
      virtual void close();

      /**
       * @brief Performs all server operations
       * 
       * Wait at most \c timeoutMs ms for a request from a client and then 
       * perform the necessary operations before responding.
       * 
       * @return return the number of Modbus data of the response if successful.
       * Otherwise it shall return -1 and set errno.
       */
      int poll (long timeoutMs = 0);

      /**
       * @brief Adds a slave 
       *
       * This function shall add a slave with the \b slaveAddr value.
       *
       * The behavior depends of network and the role of the device:
       * - \b RTU: Define the slave ID of the remote device to talk in master
       *  mode or set the internal slave ID in slave mode. According to the
       *  protocol, a Modbus device must only accept message holding its slave
       *  number or the special broadcast number.
       * - \b TCP: The slave number is only required in TCP if the message must
       *  reach a device on a serial link. Some not compliant devices or
       *  software (such as modpoll) uses the slave ID as unit identifier,
       *  that's incorrect (cf page 23 of Modbus Messaging Implementation
       *  Guide v1.0b) but without the slave value, the faulty remote device or
       *  software drops the requests ! \n
       *  The special value \b TcpSlave (255) can be used in TCP mode
       *  to restore the default value.
       * .
       *
       * @return the slave by reference
       * @sa slave()
       */
      BufferedSlave & addSlave (int slaveAddr, Device * master = 0);

      /**
       * @brief Returns the slave whose address is provided.
       * 
       * The slave must have been added with \c addSlave() else a 
       * std::out_of_range exception is thrown.
       * 
       * If the Device that drives the slave to an RTU backend, we can access 
       * to the general call through the slave at address 0 
       * (added by the constructor).
       * 
       * If the Device that drives the slave to a TCP backend, we can access to 
       * TCP messages through the slave at address 255 (added by the constructor).
       */
      BufferedSlave & slave (int slaveAddr = -1);

      /**
       * @overload 
       */
      const BufferedSlave & slave (int slaveAddr = -1) const;

      /**
       * @brief Returns a pointer to the slave whose address is provided.
       * 
       * The slave must have been added with \c addSlave() else a 
       * a nullptr is returned.
       * 
       * If the Device that drives the slave to an RTU backend, we can access 
       * to the general call through the slave at address 0 
       * (added by the constructor).
       * 
       * If the Device that drives the slave to a TCP backend, we can access to 
       * TCP messages through the slave at address 255 (added by the constructor).
       */
      BufferedSlave * slavePtr (int slaveAddr = -1);

      /**
       * @overload 
       */
      const BufferedSlave * slavePtr (int slaveAddr = -1) const;

      /**
       * @brief Slave table access operator
       * 
       * server being an object of class Server, \c server[i] is equivalent to
       * \c server.slave(i)
       */
      BufferedSlave &operator[] (int);

      /**
       * @overload 
       */
      const BufferedSlave &operator[] (int) const;

      /**
       * @brief Check if the slave at the given address \c slaveAddrexists
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
