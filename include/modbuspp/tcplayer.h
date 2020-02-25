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

#include <modbuspp/netlayer.h>

namespace Modbus {

  /**
   * @class TcpLayer
   * @brief TCP/IP v4 & v6 network layer
   *
   * This class can not and should not be instantiated by the user.
   * It provides access to properties and methods specific to the TCP layer.
   *
   * An instance of this class is created by the constructor @b Device::Device()
   * of the @b Device class (or its derived classes) if the TCP layer is selected.
   *
   * Access to this instance is done using the Device::tcp() method.
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   *
   * @sa Device::Device()
   * @sa Device::tcp()
   */
  class TcpLayer : public NetLayer {

  public:
      /**
       * @brief Constructor
       */
      TcpLayer (const std::string & host, const std::string & service);

      /**
       * @brief Host name or IP address
       *
       * This property specifies the host name or IP address of the host to
       * which we are connected, eg. "192.168.0.5" , "::1" or "server.com".
       */
      const std::string & node() const;

      /**
       * @brief Service name or port number
       *
       * This property is the service name/port number to which we are connected.
       */
      const std::string & service() const;

      /**
       * @overload
       */
      virtual int sendRawMessage (const Message * msg);
      
      /**
       * @overload
       */
      virtual bool prepareToSend (Message & msg);

    protected:
      class Private;
      TcpLayer (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (TcpLayer)
  };
}

/* ========================================================================== */
