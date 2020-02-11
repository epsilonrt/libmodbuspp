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
#include <modbuspp/message.h>

namespace Modbus {

  
  /**
   * @class VirtualRtuLayer
   * @brief Virtual RTU layer
   *
   * This class can not and should not be instantiated by the user.
   * It provides access to properties and methods specific to the RTU layer.
   *
   * An instance of this class is created by the constructor \b Device::Device()
   * of the \b Device class (or its derived classes) if the RTU layer is selected.
   *
   * Access to this instance is done using the Device::virtualRtu() method.
   *
   * @sa Device::Device()
   * @sa Device::virtualRtu()
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class VirtualRtuLayer : public NetLayer {
    public:

      /**
       * @brief Constructor
       */
      VirtualRtuLayer (const std::string & name = "virtual");

      /**
       * @brief Push ADU data to the device
       */
      bool push (const Message & adu);
      
      /**
       * @brief Pull ADU data from the device
       */
      bool pull (Message & adu);

    protected:
      class Private;
      VirtualRtuLayer (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (VirtualRtuLayer)
  };
}

/* ========================================================================== */
