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

#include <nlohmann/json.hpp>
#include <modbuspp/global.h>

namespace Modbus {
  class Slave;
  class BufferedSlave;
  class Device;
  class Master;
  class Server;
  class Router;

  NLOHMANN_JSON_SERIALIZE_ENUM (Table, {
    {DiscreteInput, "discrete-input"},
    {Coil, "coil"},
    {InputRegister, "input-register"},
    {HoldingRegister, "holding-register"},
  })

  NLOHMANN_JSON_SERIALIZE_ENUM (Net, {
    {Rtu, "rtu"},
    {Tcp, "tcp"},
    {NoNet, nullptr},
  })
  
  NLOHMANN_JSON_SERIALIZE_ENUM (SerialMode, {
    {Rs232, "rs232"},
    {Rs485, "rs485"},
    {UnknownMode, nullptr},
  })
  
  NLOHMANN_JSON_SERIALIZE_ENUM (SerialRts, {
    {RtsNone, "none"},
    {RtsUp, "up"},
    {RtsDown, "down"},
    {UnknownRts, nullptr},
  })

  namespace Json {
    void setConfig (Slave * s, const nlohmann::json & config);
    void setConfig (BufferedSlave * s, const nlohmann::json & config);
    void setConfig (Device * s, const nlohmann::json & config);
    void setConfig (Master * s, const nlohmann::json & config);
    
    void setConfig (Server * s, const nlohmann::json & config);
    void setConfig (Router * s, const nlohmann::json & config);
  }
}

/* ========================================================================== */
