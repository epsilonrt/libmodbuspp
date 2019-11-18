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
#include <modbuspp/global.h>

namespace Modbus {
  class Device;
  class NetLayer;

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
      typedef int (*Callback) (Message & msg, Device * sender);

      /**
       * @brief Constructors
       */
      Message (NetLayer & backend);
      Message (Device & dev);
      Message (NetLayer & backend, const std::vector<uint8_t> & adu);
      Message (Device & dev, const std::vector<uint8_t> & adu);
      Message (NetLayer & backend, Function func);
      Message (Device & dev, Function func);

      /**
       * @brief Destructor
       */
      virtual ~Message();

      std::vector<uint8_t> & adu ();
      const std::vector<uint8_t> & adu () const;
      uint8_t adu (uint16_t i);

      uint8_t byte (int pduOffset) const;
      uint16_t word (int pduOffset) const;
      void setByte (int pduOffset, uint8_t value);
      void setWord (int pduOffset, uint16_t value);

      int slaveId() const;
      Function function() const;
      uint16_t startingAddress() const;
      uint16_t quantity() const;
      uint16_t value (uint16_t index) const;

      void setFunction (Function func);
      void setSlaveId (int id);
      void setStartingAdress (uint16_t addr);
      void setQuantity (uint16_t n);

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

    protected:
      class Private;
      Message (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Message)
  };
}

/* ========================================================================== */
