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

#include <modbuspp/message.h>

namespace Modbus {
 
  /**
   * @class Request
   * @brief Modbus Request Message
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Request : public Message  {
    public:

      /**
       * @brief Constructors
       */
      explicit Request (NetLayer & backend);
      explicit Request (Device & dev);
      Request (NetLayer & backend, const std::vector<uint8_t> & adu);
      Request (Device & dev, const std::vector<uint8_t> & adu);
      Request (NetLayer & backend, const uint8_t * adu, size_t adulength);
      Request (Device & dev, const uint8_t * adu, size_t adulength);
      Request (NetLayer & backend, Function func);
      Request (Device & dev, Function func);

      /**
       * @brief Constructs a copy of @b other
       */
      Request (const Request & other);
      
      /**
       * @brief Move Constructor
       * The object acquires the content managed by @b other. 
       * @b other becomes an empty object (as if default-constructed).
       */
      Request (Request && other);

      /**
       * @brief Returns the byte count of the request
       *
       * Can be used for the functions Write Multiple Coils(15) and 
       * Write Multiple Registers(16).
       * This value is at the pdu[5].
       */
      uint8_t byteCount () const;

      /**
       * @brief Returns the register value of the request 
       * 
       * Can be used for function Write Single Register(06).
       * This value is at the pdu[3].
       */
      uint16_t registerValue () const;

      /**
       * @brief Returns a register value of the request
       * 
       * Can be used for function Write Multiple registers(16).
       * This value is at the pdu[6+index].
       */
      uint16_t registerValue (uint16_t index) const;

      /**
       * @brief Returns a register values of the request
       * 
       * Can be used for function Write Multiple registers(16).
       * This values are at the pdu[6+index].
       */
      void registerValues (uint16_t index, uint16_t quantity, uint16_t * values) const;

      /**
       * @brief Returns the coil value of the request 
       * 
       * Can be used for function Write Single Coil(05).
       * This value is at the pdu[3].
       */
      bool coilValue () const;

      /**
       * @brief Returns a coil value of the request
       * 
       * Can be used for function Write Multiple Coils(15).
       * This values are at the pdu[6+index/8].
       */
      bool coilValue (uint16_t index) const;

      /**
       * @brief Returns coil values of the request
       * 
       * Can be used for function Write Multiple Coils(15).
       * This values are at the pdu[6+index/8].
       */
      void coilValues (uint16_t index, uint16_t quantity, bool * values) const;

      /**
       * @brief Sets the byte count for the request
       *
       * Can be used for the functions Write Multiple Coils(15) and 
       * Write Multiple Registers(16).
       * This value is at the pdu[5].
       */
      void setByteCount (uint8_t n);

      /**
       * @brief Sets the register value for the request
       *
       * Can be used for function Write Single Register(06).
       * This value is at the pdu[3].
       */
      void setRegisterValue (uint16_t value);

      /**
       * @brief Sets a register value for the request
       *
       * Can be used for function Write Multiple registers(16).
       * This value is at the pdu[6+index].
       */
      void setRegisterValue (uint16_t index, uint16_t value);

      /**
       * @brief Sets register values for the request
       *
       * Can be used for function Write Multiple registers(16).
       * This value is at the pdu[6+index].
       */
      void setRegisterValues (uint16_t index, uint16_t quantity, const uint16_t * values);

      /**
       * @brief Sets the coil value for the request
       *
       * Can be used for function Write Single Coil(05).
       * This value is at the pdu[3].
       */
      void setCoilValue (bool value);

      /**
       * @brief Sets the coil value for the request
       *
       * Can be used for function Write Multiple Coils(15).
       * This values are at the pdu[6+index/8].
       */
      void setCoilValue (uint16_t index, bool value);

      /**
       * @brief Sets register values for the request
       *
       * Can be used for function Write Multiple Coils(15).
       * This values are at the pdu[6+index/8].
       */
      void setCoilValues (uint16_t index, uint16_t quantity, const bool * values);

    protected:
      class Private;
      Request (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Request)
  };
}

/* ========================================================================== */
