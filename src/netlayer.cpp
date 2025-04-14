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
#include "netlayer_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                          NetLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  NetLayer::NetLayer (std::unique_ptr<NetLayer::Private> &&dd) : d_ptr (std::move(dd)) {}

  // ---------------------------------------------------------------------------
  NetLayer::NetLayer () :
    d_ptr (new Private (NoNet, std::string(), std::string(), 0)) {}

  // ---------------------------------------------------------------------------
  NetLayer::~NetLayer() = default;

  // ---------------------------------------------------------------------------
  uint16_t NetLayer::maxAduLength() const {
    PIMP_D (const NetLayer);

    return d->maxAduLength;
  }

  // ---------------------------------------------------------------------------
  Net NetLayer::net() const {
    PIMP_D (const NetLayer);

    return d->net;
  }

  // ---------------------------------------------------------------------------
  modbus_t * NetLayer::context() {
    PIMP_D (NetLayer);

    return d->ctx.get();
  }

  // ---------------------------------------------------------------------------
  const modbus_t * NetLayer::context() const {
    PIMP_D (const NetLayer);

    return d->ctx.get();
  }

  // ---------------------------------------------------------------------------
  const std::string & NetLayer::connection() const {
    PIMP_D (const NetLayer);

    return d->connection;
  }

  // ---------------------------------------------------------------------------
  const std::string & NetLayer::settings() const {
    PIMP_D (const NetLayer);

    return d->settings;
  }

  // ---------------------------------------------------------------------------
  int NetLayer::sendRawMessage (const Message * msg) {

    errno = EINVAL;
    return -1;
  }

  // ---------------------------------------------------------------------------
  bool NetLayer::prepareToSend (Message & msg) {

    return false;
  }

  // ---------------------------------------------------------------------------
  bool NetLayer::checkMessage (const Message & msg) {

    return false;
  }
  
  // ---------------------------------------------------------------------------
  // static
  std::string NetLayer::lastError() {

    return modbus_strerror (errno);
  }
}

/* ========================================================================== */
