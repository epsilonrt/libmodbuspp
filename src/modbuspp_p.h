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

#ifndef MODBUSPP_PRIVATE_H
#define MODBUSPP_PRIVATE_H

#include <cerrno>
#include <modbuspp.h>
#include <modbus.h>

namespace Modbus {

  class NetLayer::Private {
    public:
      Private (modbus_t * ctx) : ctx (ctx) {}

      modbus_t * ctx;
  };

  class TcpLayer::Private  : public NetLayer::Private {

    public:
      Private (modbus_t * ctx, const std::string & host, const std::string & service) :
        NetLayer::Private (ctx), host (host), service (service) {}

      std::string host;
      std::string service;
  };

  class RtuLayer::Private  : public NetLayer::Private {

    public:
      Private (modbus_t * ctx, const std::string & port, const std::string & settings) :
        NetLayer::Private (ctx), port (port), settings (settings) {}
        
      std::string port;
      std::string settings;
  };

  class Device::Private {

    public:
      Private (Device * q, Net net, const std::string & connection, const std::string & settings);
      virtual ~Private();
      int address (int addr);

      Device * const q_ptr;
      modbus_t * ctx;
      bool isOpen;
      bool pduAddressing;
      RtuLayer * rtu;
      TcpLayer * tcp;
      int slave;
      
      friend class RtuLayer::Private;
      friend class TcpLayer::Private;

      PIMP_DECLARE_PUBLIC (Device)
  };

  class Master::Private : public Device::Private {

    public:
      Private (Master * q, Net net, const std::string & connection, const std::string & settings);
      virtual ~Private();

      PIMP_DECLARE_PUBLIC (Master)
  };
}

/* ========================================================================== */
#endif /* MODBUSPP_PRIVATE_H defined */
