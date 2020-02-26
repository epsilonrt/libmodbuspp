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
  class Request;
  
  /**
   * @class Response
   * @brief Modbus Response Message
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Response : public Message  {
    public:

      /**
       * @brief Constructors
       */
      explicit Response (NetLayer & backend);
      explicit Response (Device & dev);
      Response (NetLayer & backend, const std::vector<uint8_t> & adu);
      Response (Device & dev, const std::vector<uint8_t> & adu);
      Response (NetLayer & backend, const uint8_t * adu, size_t adulength);
      Response (Device & dev, const uint8_t * adu, size_t adulength);
      Response (NetLayer & backend, Function func);
      Response (Device & dev, Function func);

      /**
       * @brief Copy Constructors
       */
      Response (const Request & req);
      Response (const Message & msg);
      Response (const Response & other);
      
      /**
       * @brief Move Constructors
       */
      Response (Request && req);
      Response (Message && msg);
      Response (Response && other);

      /**
       * @brief Sets the byte count for the response
       *
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02),
       * Read Holding Registers(03), Read Input Register(04).
       * This value is at the pdu[1].
       */
      void setByteCount (uint8_t n);

      /**
       * @brief Sets a register value for the response
       *
       * Can be used for the functions Read Holding Registers(03) and 
       * Read Input Register(04).
       * This value is at the pdu[2 + index*2].
       */
      void setRegisterValue (uint16_t index, uint16_t value);

      /**
       * @brief Sets register values for the response
       *
       * Can be used for the functions Read Holding Registers(03) and 
       * Read Input Register(04).
       * This value is at the pdu[2 + index*2].
       */
      void setRegisterValues (uint16_t index, uint16_t quantity, const uint16_t * values);

      /**
       * @brief Sets a bit value for the response
       *
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02).
       * This value is at the pdu[2 + index*2].
       */
      void setBitValue (uint16_t index, bool value);

      /**
       * @brief Sets bit values for the response
       *
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02).
       * This value is at the pdu[2 + index*2].
       */
      void setBitValues (uint16_t index, uint16_t quantity, const bool * values);

      /**
       * @brief Sets the exception code for the response
       *
       * Can be used for all functions.
       * This value is at the pdu[1].
       */
      void setExceptionCode (ExceptionCode code);

      /**
       * @brief Returns the exception code of the response
       *
       * Can be used for all functions.
       * This value is at the pdu[1].
       */
      ExceptionCode exceptionCode() const;

      /**
       * @brief Returns a register value of the response
       * 
       * Can be used for the functions Read Holding Registers(03) and 
       * Read Input Register(04).
       * This value is at the pdu[2 + index*2].
       */
      uint16_t registerValue (uint16_t index) const;

      /**
       * @brief Returns register values of the response
       * 
       * Can be used for the functions Read Holding Registers(03) and 
       * Read Input Register(04).
       * This value is at the pdu[2 + index*2].
       */
      void registerValues (uint16_t index, uint16_t quantity, uint16_t * values) const;
      
      /**
       * @brief Returns a bit value of the response
       * 
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02).
       * This value is at the pdu[2 + index*2].
       */
      bool bitValue (uint16_t index) const;

      /**
       * @brief Returns bit values of the response
       * 
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02).
       * This value is at the pdu[2 + index*2].
       */
      void bitValues (uint16_t index, uint16_t quantity, bool * values) const;
      
    protected:
      class Private;
      Response (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Response)
  };

}

/* ========================================================================== */
