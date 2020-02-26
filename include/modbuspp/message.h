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

#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <modbuspp/global.h>
#include <modbuspp/data.h>

namespace Modbus {
  class Device;
  class NetLayer;
  class Request;
  class Response;

  /**
   * @class Message
   * @brief Modbus Message
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Message  {
    public:

      friend class Device;
      /**
       * @brief Message callback function
       * @param msg
       * @return 1 if the message has been completely processed, 0 if the
       * message has not been processed, -1 if error.
       */
      typedef int (*Callback) (Message * msg, Device * sender);

      /**
       * @brief Constructors
       */
      explicit Message (NetLayer & backend);
      explicit Message (Device & dev);
      Message (NetLayer & backend, const std::vector<uint8_t> & adu);
      Message (Device & dev, const std::vector<uint8_t> & adu);
      Message (NetLayer & backend, const uint8_t * adu, size_t adulength);
      Message (Device & dev, const uint8_t * adu, size_t adulength);
      Message (NetLayer & backend, Function func);
      Message (Device & dev, Function func);

      /**
       * @brief Constructs a copy of @b other
       */
      Message (const Message & other);
      
      /**
       * @brief Move Constructor
       * The object acquires the content managed by @b other. 
       * @b other becomes an empty object (as if default-constructed).
       */
      Message (Message && other);

      /**
       * @brief Sets the Message object to be equal to @b other.
       */
      Message& operator= (const Message &other);
      
      /**
       * @brief Move assignment
       */
      Message& operator= (Message && other);

      /**
       * @brief Swaps Message @b other with this Message. This operation is
       * very fast and never fails.
       */
      void swap (Message &other);

      /**
       * @brief Equality operator
       */
      bool operator== (const Message & other);

      /**
       * @brief Difference operator
       */
      bool operator!= (const Message & other);

      /**
       * @brief Destructor
       */
      virtual ~Message();

      /**
       * @brief Returns the number of the slave concerned by the request.
       *
       * Can be used for all functions.
       * This value is at the offset[-1] of the PDU
       */
      int slave() const;

      /**
       * @brief Returns the function of the request.
       *
       * Can be used for all functions.
       * This value is at the offset[0] of the PDU
       */
      Function function() const;

      /**
       * @brief Returns the starting address of the request.
       *
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02),
       * Read Holding Registers(03), Write Multiple Coils(15) and
       * Write Multiple Registers(16).
       * This value is at the offset[1] of the PDU
       */
      uint16_t startingAddress() const;

      /**
       * @brief Returns the number of elements of the request.
       *
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02),
       * Read Holding Registers(03), Write Multiple Coils(15) and
       * Write Multiple Registers(16).
       * This value is at the offset[3] of the PDU
       */
      uint16_t quantity() const;

      /**
       * @brief Sets the number of the slave concerned by the request.
       *
       * Can be used for all functions.
       * This value is at the offset[-1] of the PDU
       */
      void setSlaveId (int id);

      /**
       * @brief Sets the function of the request.
       *
       * Can be used for all functions.
       * This value is at the offset[0] of the PDU
       */
      void setFunction (Function func);

      /**
       * @brief Sets the starting address of the request.
       *
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02),
       * Read Holding Registers(03), Read Input Register(04), Write Multiple
       * Coils(15) and Write Multiple Registers(16).
       * This value is at the offset[1] of the PDU
       */
      void setStartingAdress (uint16_t addr);

      /**
       * @brief Sets the number of elements of the request.
       *
       * Can be used for the functions Read Coils(01), Read Discrete Inputs(02),
       * Read Holding Registers(03), Read Input Register(04), Write Multiple
       * Coils(15) and Write Multiple Registers(16).
       * This value is at the offset[3] of the PDU
       */
      void setQuantity (uint16_t n);

      /**
       * @brief Returns the PDU byte corresponding to the offset provided
       *
       * @b pduOffset has a maximum value of @b MaxPduLength-1, it can have a negative
       * value in order to access the bytes of the ADU that precedes the PDU.
       */
      uint8_t byte (int pduOffset) const;

      /**
       * @brief Returns the PDU word corresponding to the offset provided
       *
       * @b pduOffset has a maximum value of @b MaxPduLength-2, it can have a negative
       * value in order to access the words of the ADU that precedes the PDU.
       */
      uint16_t word (int pduOffset) const;

      /**
       * @brief Write a byte in the PDU to the supplied offset
       *
       * @b pduOffset has a maximum value of @b MaxPduLength-1, it can have a negative
       * value in order to access the bytes of the ADU that precedes the PDU.
       */
      void setByte (int pduOffset, uint8_t value);

      /**
       * @brief Write a word in the PDU to the supplied offset
       *
       * @b pduOffset has a maximum value of @b MaxPduLength-2, it can have a negative
       * value in order to access the wordss of the ADU that precedes the PDU.
       */
      void setWord (int pduOffset, uint16_t value);

      /**
       * @brief Empties the message and returns it to the initial state
       */
      void clear();

      /**
       * @brief returns the number of bytes of the PDU
       * 
       * @note Use @b aduSize() to get the number of bytes of the ADU.
       */
      size_t size() const;
      
      /**
       * @brief Change the size of the PDU
       * 
       * Enables the size of the PDU to be forced if the ADU buffer contains 
       * information that must be kept.
       * 
       */
      void setSize(size_t size);

      /**
       * @brief Returns the underlying ADU bytes pointer
       */
      uint8_t * adu ();

      /**
       * @overload
       */
      const uint8_t * adu () const;

      /**
       * @brief Returns the ADU byte corresponding to the index provided
       *
       * @b i must be between 0 and @b maxAduLength()-1
       */
      uint8_t adu (uint16_t i);

      /**
       * @brief returns the number of bytes of the ADU
       * 
       */
      size_t aduSize() const;
      
      /**
       * @brief Change the size of the ADU
       * 
       * Enables the size of the PDU to be forced if the ADU buffer contains 
       * information that must be kept.
       * 
       */
      void setAduSize(size_t size);

      /**
       * @brief Return the ADU header length
       */
      uint16_t aduHeaderLength() const;

      /**
       * @brief Underlying layer used (backend)
       *
       * This function allows to know the underlying layer used.
       */
      Net net() const;

      /**
       * @brief Return the maximum ADU length
       */
      uint16_t maxAduLength() const;

      
      /**
       * @brief Prints all bytes of the message on the output stream provided
       * 
       * The bytes printed are preceded by the character '<'  and 
       * followed '>' if the message is a request, and preceded by the character 
       * '['  and followed '[' if the message is a response.
       * 
       * for debugging.
       */
      void print (std::ostream& os) const;

      /**
       * @brief Prints all bytes of the message on the output stream provided
       * 
       * The bytes printed are preceded by the character @b prefix and 
       * followed by the suffix.
       * 
       * for debugging.
       */
      void print (std::ostream& os, char prefix, char suffix) const;
      
      /**
       * @brief Returns true if the message is a response
       */
      bool isResponse() const;
      
      /**
       * @brief Inserts the message @b m byte sequence into @b os.
       */
      friend std::ostream& operator<< (std::ostream& os, const Message & m);

    protected:
      class Private;
      Message (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Message)
  };
}

/* ========================================================================== */
