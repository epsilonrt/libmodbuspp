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
#include <modbuspp/data.h>
#include <modbuspp/bufferedslave.h>

namespace Modbus {
  class Slave;
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

  NLOHMANN_JSON_SERIALIZE_ENUM (DataEnum, {
    {Uint16, "uint16"},
    {Uint32, "uint32"},
    {Uint64, "uint64"},
    {Int16, "int16"},
    {Int32, "int32"},
    {Int64, "int64"},
    {Float, "float"},
    {Double, "double"},
    {LongDouble, "longdouble"},
  })

  NLOHMANN_JSON_SERIALIZE_ENUM (Endian, {
    {EndianBigBig, "abcd"},
    {EndianBigLittle, "cdab"},
    {EndianLittleBig, "badc"},
    {EndianLittleLittle, "dcba"},
  })

  namespace Json {
    void setConfig (Slave * s, const nlohmann::json & config);
    void setConfig (BufferedSlave * s, const nlohmann::json & config);
    void setConfig (Device * s, const nlohmann::json & config);
    void setConfig (Master * s, const nlohmann::json & config);
    
    void setConfig (Server * s, const nlohmann::json & config);
    void setConfig (Router * s, const nlohmann::json & config);

    int writeBits (BufferedSlave * s, const nlohmann::json & block);
    int getBitValues (std::vector<uint8_t> & data, int nmemb, const nlohmann::json & values);
    int writeRegisters (BufferedSlave * s, const nlohmann::json & block);

    //--------------------------------------------------------------------------
    template <typename T>
    int getDataValues (std::vector<Data<T>> & data, Endian endian,
                       const nlohmann::json & values) {

      for (const auto & v : values) {
        Data<T> d;
        d.setEndianness (endian);

        if (v.is_string()) {
          auto s = v.get<std::string>();

          try {

            strToT (d.value(), s);
          }
          catch (...) {

            d = 0;
          }
        }
        else {

          auto t = v.get<T>();
          d = t;
        }
        data.push_back (d);
      }
      return data.size();
    }

    //--------------------------------------------------------------------------
    template <typename T>
    int writeRegisters (BufferedSlave * s, const nlohmann::json & block) {
      const auto & values = block["values"];
      auto nmemb = block["quantity"].get<int>(); // mandatory

      if (values.is_array() && values.size() > 0 && nmemb > 0) {
        std::vector<Data<T>> vect;
        Endian endian = EndianBig;

        if (block.contains ("endian")) {

          block["endian"].get_to (endian);
        }

        if (getDataValues (vect, endian, values) > 0) {
          auto table = block["table"].get<Table>(); // mandatory
          int addr = s->dataAddress (0);

          if (block.contains ("values-address")) {

            block["values-address"].get_to (addr);
          }

          if (table == HoldingRegister) {

            return s->writeRegisters (addr, vect.data(), vect.size());
          }
          else {

            return s->writeInputRegisters (addr, vect.data(), vect.size());
          }
        }
      }
      return 0;
    }
  }
}

/* ========================================================================== */
