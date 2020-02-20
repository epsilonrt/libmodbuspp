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

#include <modbuspp/server.h>

namespace Modbus {

  /**
    * @class Router
    * @brief Router connected to Modbus (Router)
    *
    * The Modbus slave is waiting for request from Modbus Masters (clients) and
    * must answer when it is concerned by the request.
    *
    * To use, simply perform the following actions:
    *
    * @code
       // instantiate a variable by choosing the network and the parameters to connect to it
       Router srv (net, connection, settings);

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
  class Router : public Server {

    public:
      /**
       * @brief Constructor
       *
       * Constructs a Modbus router for the @b net network.
       *
       * For the Tcp backend :
       * - @b connection specifies the host name or IP
       * address of the host to connect to, eg. "192.168.0.5" , "::1" or
       * "server.com". A NULL value can be used to listen any addresses in server mode,
       * - @b settings is the service name/port number to connect to.
       * To use the default Modbus port use the string "502". On many Unix
       * systems, it’s convenient to use a port number greater than or equal
       * to 1024 because it’s not necessary to have administrator privileges.
       * .
       *
       * For the Rtu backend :
       * - @b connection specifies the name of the serial port handled by the
       *  OS, eg. "/dev/ttyS0" or "/dev/ttyUSB0",
       * - @b settings specifies communication settings as a string in the
       *  format BBBBPS. BBBB specifies the baud rate of the communication, PS
       *  specifies the parity and the bits of stop.
       * 
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
      Router (Net net, const std::string & connection,
              const std::string & settings);

      /**
       * @brief constructor from a JSON file
       *
       * The file describes the configuration to apply, its format is as follows:
       *
       * @code
        {
          "modbuspp-router": {
            "mode": "tcp",
            "connection": "localhost",
            "settings": "1502",
            "recovery-link": true,
            "debug": true,
            "response-timeout": 500,
            "byte-timeout": 500,
            "masters": [
              {
                "name": "rs485",
                "mode": "rtu",
                "connection": "/dev/ttyS1",
                "settings": "38400E1",
                "debug": true,
                "response-timeout": 500,
                "byte-timeout": 500,
                "rtu": {
                  "mode": "rs485",
                  "rts": "down"
                },
                "slaves": [
                  {
                    "id": 33,
                    "pdu-adressing": false,
                    "blocks": [
                      {
                        "table": "input-register",
                        "starting-address": 1,
                        "quantity": 6
                      },
                      {
                        "table": "holding-register",
                        "starting-address": 1,
                        "quantity": 6
                      }
                    ]
                  }
                ]
              },
              {
                "name": "virtual",
                "mode": "rtu",
                "connection": "/dev/tnt0",
                "settings": "38400E1",
                "debug": true,
                "response-timeout": 3000,
                "byte-timeout": 500
              }
            ]
          }
        }
       * @endcode
       *
       * Only the first 3 elements are mandatory : @b mode, @b connection and @b settings,
       * the others are optional. In this example "modbuspp-router" is the key
       * to the JSON object that should be used. If the key provided is empty,
       * the file contains only one object corresponding to the configuration.
       *
       * For slave objects, only the @b id field is required.
       *
       * In blocks objects, the @b table and @b quantity members are the only mandatory.
       *
       * @param jsonfile JSON file path
       * @param key name of the object key in the JSON file corresponding to
       * the configuration to be applied
       */
      explicit Router (const std::string & jsonfile, const std::string & key = std::string());

      /**
       * @brief Default constructor
       *
       * object cannot be used without calling @b setBackend() or @b setConfig()
       */
      Router ();

      /**
       * @brief Destructor
       *
       * The destructor closes the connection if it is open and releases all
       * affected resources.
       */
      virtual ~Router();

      /**
       * @overload
       */
      virtual void close();

      /**
       * @brief Adds a slave
       *
       * This function shall add a slave with the @b slaveAddr value.
       *
       * The behavior depends of network and the role of the device:
       * - @b RTU: Define the slave ID of the remote device to talk in master
       *  mode or set the internal slave ID in slave mode. According to the
       *  protocol, a Modbus device must only accept message holding its slave
       *  number or the special broadcast number.
       * - @b TCP: The slave number is only required in TCP if the message must
       *  reach a device on a serial link. Some not compliant devices or
       *  software (such as modpoll) uses the slave ID as unit identifier,
       *  that's incorrect (cf page 23 of Modbus Messaging Implementation
       *  Guide v1.0b) but without the slave value, the faulty remote device or
       *  software drops the requests !
       * 
       *  The special value @b TcpSlave (255) can be used in TCP mode
       *  to restore the default value.
       * .
       *
       * @return the slave by reference
       * @sa slave()
       */
      Master & addMaster (const std::string & name,
                          Net net, const std::string & connection,
                          const std::string & settings);

      /**
       * @overload
       */
      Master & addMaster (const std::string & name);

      /**
       * @brief Returns the slave whose address is provided.
       *
       * The slave must have been added with @b addSlave() else a
       * std::out_of_range exception is thrown.
       *
       * If the Device that drives the slave to an RTU backend, we can access
       * to the general call through the slave at address 0
       * (added by the constructor).
       *
       * If the Device that drives the slave to a TCP backend, we can access to
       * TCP messages through the slave at address 255 (added by the constructor).
       */
      Master & master (const std::string & name);

      /**
       * @overload
       */
      const Master & master (const std::string & name) const;

      /**
       * @brief Returns a pointer to the slave whose address is provided.
       *
       * The slave must have been added with @b addSlave() else a
       * a nullptr is returned.
       *
       * If the Device that drives the slave to an RTU backend, we can access
       * to the general call through the slave at address 0
       * (added by the constructor).
       *
       * If the Device that drives the slave to a TCP backend, we can access to
       * TCP messages through the slave at address 255 (added by the constructor).
       */
      Master * masterPtr (const std::string & name);

      /**
       * @overload
       */
      const Master * masterPtr (const std::string & name) const;

      /**
       * @brief Slave table access operator
       *
       * server being an object of class Router, @b server[i] is equivalent to
       * @b server.slave(i)
       */
      Master &operator[] (const std::string &);

      /**
       * @overload
       */
      const Master &operator[] (const std::string &) const;

      /**
       * @brief Check if the slave at the given address @b slaveAddrexists
       */
      bool hasMaster (const std::string & name) const;

      /**
       * @brief Returns the list of masters as a map indexed by name 
       */
      const std::map <std::string, std::shared_ptr<Master>> & masters() const;

    protected:
      class Private;
      Router (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Router)
  };
}

/* ========================================================================== */
