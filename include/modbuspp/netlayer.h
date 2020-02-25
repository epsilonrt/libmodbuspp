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
#include <modbuspp/global.h>

namespace Modbus {
  class Message;
  
  /**
   * @class NetLayer
   * @brief Network layer base class (backend)
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class NetLayer  {
    public:

      /**
       * @brief Constructor
       */
      NetLayer();

      /**
       * @brief Destructor
       */
      virtual ~NetLayer();

      /**
       * @brief Underlying layer used (backend)
       *
       * This function allows to know the underlying layer used.
       */
      Net net() const;

      /**
       * @brief Returns the maximum ADU length
       */
      uint16_t maxAduLength() const;

      /**
       * @brief Returns the connection used
       * 
       * Serial port or host depending on the backend.
       */
      const std::string & connection() const;
      
      /**
       * @brief Returns the connection settings
       * 
       * IP port or speed, parity and stop bit depending on the backend
       */
      const std::string & settings() const;

      /**
       * @brief libmodbus context
       *
       * context is an opaque structure containing all necessary information to
       * establish a connection with other Modbus devices according to the
       * selected variant.
       */
      modbus_t * context();
      
      /**
       * @overload
       */
      const modbus_t * context() const;
      
      /**
       * @brief Send a request/response @b msg  via the socket of the @b context()
       * 
       * This function can be used to send message not handled by the library.
       * The message is transmitted "raw", without any modification.
       * 
       * @return The function shall return the full message length, if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      virtual int sendRawMessage (const Message * msg);

      /**
       * @brief Prepare the message @b msg before sending
       * 
       * This function updates the ADU header or adds the CRC at the end 
       * depending on the network used..
       */
      virtual bool prepareToSend (Message & msg);

      /**
       * @brief last error message
       *
       * This function shall return the error message corresponding to
       * the last error. This function must be called right after the
       * instruction that triggered an error.
       */
      static std::string lastError();

    protected:
      class Private;
      NetLayer (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (NetLayer)
  };
}

/* ========================================================================== */
