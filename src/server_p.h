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
#include <future>
#include <thread>
#include <modbuspp/server.h>
#include "device_p.h"

namespace Modbus {

  class Server::Private : public Device::Private {

    public:
      Private (Server * q);
      virtual ~Private();
      virtual void setBackend (Net net, const std::string & connection,
                               const std::string & settings);
      virtual void setConfig (const nlohmann::json & config);

      virtual bool open();
      virtual void close();
      int task (int rc);

      BufferedSlave * addSlave (int slaveAddr, Device * master);

      static void loop (std::future<void>&& run, Private * d);
      static int receive (Private * d);

      int sock;
      std::shared_ptr<Request> req;
      std::map <int, std::shared_ptr<BufferedSlave>> slave;
      std::future<int> receiveTask;
      std::thread daemon;
      std::promise<void> stopDaemon;
      Message::Callback messageCB;

      PIMP_DECLARE_PUBLIC (Server)
  };
}

/* ========================================================================== */
