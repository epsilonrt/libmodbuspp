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

#include <map>
#include <modbuspp/device.h>
#include <modbuspp/slave.h>

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
      // adds a new slave and gets her reference
      Slave & slave = mb.addSlave(33);
      // open the communication
      mb.open ();
      // perform read or write operations for this slave
      slave.readInputRegisters (1, values, 2);
   * @endcode
   *
   * @example master/read-coils/main.cpp
   * @example master/read-holding-data/main.cpp
   * @example master/report-slave-id/main.cpp
   * @example master/read-input-registers/main.cpp
   * @example master/rw-holding-json/main.cpp
   * @example master/write-holding-data/main.cpp
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Master : public Device {

    public:
      /**
       * @overload
       */
      Master (Net net, const std::string & connection, const std::string & settings);

      /**
       * @brief constructor from a JSON file
       * 
       * The file describes the configuration to apply, its format is as follows:
       * 
       * @code
          {
            "modbuspp-master": {
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
                  "pdu-adressing": false
                },
                {
                  "id": 34,
                  "pdu-adressing": true
                }
              ]
            }
          }
       * @endcode
       * 
       * Only the first 3 elements are mandatory : @b mode, @b connection and @b settings, 
       * the others are optional. In this example "modbuspp-master" is the key 
       * to the JSON object that should be used. If the key provided is empty, 
       * the file contains only one object corresponding to the configuration.
       * 
       * For slave objects, only the @b id field is required.
       * 
       * @param key name of the object key in the JSON file corresponding to 
       * the configuration to be applied
       * @param jsonfile JSON file path
       */
      explicit Master (const std::string & jsonfile, const std::string & key = std::string());

      /**
       * @brief Default constructor
       *
       * object cannot be used without calling @b setBackend() or @b setConfig()
       */
      Master ();

      /**
       * @brief Destructor
       *
       * The destructor closes the connection if it is open and releases all
       * affected resources.
       */
      virtual ~Master();

      /**
       * @overload
       */
      bool setRecoveryLink (bool recovery = true);

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
      Slave & addSlave (int slaveAddr);
      
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
      Slave & slave (int slaveAddr = -1);
      
      /**
       * @overload 
       */
      const Slave & slave (int slaveAddr = -1) const;
      
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
      Slave * slavePtr (int slaveAddr = -1);
      
      /**
       * @overload 
       */
      const Slave * slavePtr (int slaveAddr = -1) const;
      
      /**
       * @brief Slave table access operator
       * 
       * master being an object of class Master, @b master[i] is equivalent to
       * @b master.slave(i)
       */
      Slave &operator[] (int);

      /**
       * @overload 
       */
      const Slave &operator[] (int) const;
      
      /**
       * @brief Check if the slave at the given address @b slaveAddrexists
       */
      bool hasSlave (int slaveAddr) const;
      
      /**
       * @brief Returns the list of slaves as a map indexed by identifier number 
       */
      const std::map <int, std::shared_ptr<Slave>> & slaves() const;

    protected:
      class Private;
      Master (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Master)
  };
}

/* ========================================================================== */
