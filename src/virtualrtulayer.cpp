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
#include "virtualrtulayer_p.h"
//#include "modbus-virtual-rtu.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         VirtualRtuLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  VirtualRtuLayer::VirtualRtuLayer (VirtualRtuLayer::Private &dd) : NetLayer (dd) {}

  // ---------------------------------------------------------------------------
  VirtualRtuLayer::VirtualRtuLayer (const std::string & name) :
    NetLayer (*new Private (name)) {}

  // ---------------------------------------------------------------------------
  bool VirtualRtuLayer::push (const Message & msg) {
    PIMP_D (VirtualRtuLayer);

    return (modbus_virtual_rtu_write (d->ctx, msg.adu().data(), msg.adu().size()) > 0);
  }

  // ---------------------------------------------------------------------------
  bool VirtualRtuLayer::pull (Message & msg) {
    PIMP_D (VirtualRtuLayer);

    int rc = modbus_virtual_rtu_read (d->ctx, msg.adu().data(), msg.adu().capacity());
    if (rc > 0) {
      
      msg.adu().resize (rc);
    }
    return (rc > 0);
  }

  // ---------------------------------------------------------------------------
  //
  //                    VirtualRtuLayer::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  VirtualRtuLayer::Private::Private (const std::string & name) :
    NetLayer::Private (VirtualRtu, name, std::string(), MODBUS_RTU_MAX_ADU_LENGTH) {

    ctx = modbus_new_virtual_rtu ();
    if (! ctx) {

      throw std::invalid_argument (
        "Error: Unable to create Virtual RTU Modbus Backend("
        + name + ")\n" + lastError());
    }
  }

}

/* ========================================================================== */
