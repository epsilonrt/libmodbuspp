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
#include <sstream>
#include "master_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                             Master Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Master::Master (Master::Private &dd) : Device (dd) {}

  // ---------------------------------------------------------------------------
  Master::Master (Net net, const std::string & connection,
                  const std::string & settings) :
    Device (*new Private (this, net, connection, settings)) {

    switch (net) {

      case Tcp:
        addSlave (TcpSlave);
        break;

      case VirtualRtu:
      case Rtu:
        addSlave (Broadcast);
        break;
    }
  }

  // ---------------------------------------------------------------------------
  Master::~Master() = default;

  // ---------------------------------------------------------------------------
  void Master::setRecoveryLink (bool recovery) {
    PIMP_D (Master);

    modbus_set_error_recovery (d->ctx(), recovery ?
                               MODBUS_ERROR_RECOVERY_LINK :
                               MODBUS_ERROR_RECOVERY_NONE);
    d->recoveryLink = recovery;
  }

  // ---------------------------------------------------------------------------
  bool Master::hasSlave (int slaveAddr) const {
    PIMP_D (const Master);

    return d->slave.count (slaveAddr) > 0;
  }

  // ---------------------------------------------------------------------------
  Slave & Master::addSlave (int slaveAddr) {
    PIMP_D (Master);

    if (modbus_set_slave (d->ctx(), slaveAddr) != 0) {
      std::ostringstream oss;
      
      oss << "Error: Unable to add slave[" << slaveAddr << "]\n" << lastError();
      throw std::invalid_argument (oss.str());
    }

    if (hasSlave (slaveAddr)) {
      return slave (slaveAddr);
    }
    Slave * s = new Slave (slaveAddr, this);
    d->slave[slaveAddr] = s;
    return *s;
  }

  // ---------------------------------------------------------------------------
  Slave & Master::slave (int slaveAddr) {
    PIMP_D (Master);

    return *d->slave.at (d->defaultSlave (slaveAddr));
  }

  // ---------------------------------------------------------------------------
  const Slave & Master::slave (int slaveAddr) const {
    PIMP_D (const Master);

    return *d->slave.at (d->defaultSlave (slaveAddr));
  }

  // ---------------------------------------------------------------------------
  Slave * Master::slavePtr (int slaveAddr) {
    PIMP_D (Master);
    int i = d->defaultSlave (slaveAddr);

    return hasSlave (i) ? d->slave.at (i) : 0;
  }

  // ---------------------------------------------------------------------------
  const Slave * Master::slavePtr (int slaveAddr) const {
    PIMP_D (const Master);
    int i = d->defaultSlave (slaveAddr);

    return hasSlave (i) ? d->slave.at (i) : 0;
  }

  // ---------------------------------------------------------------------------
  Slave & Master::operator[] (int slaveAddr) {
    PIMP_D (Master);

    return * d->slave.at (slaveAddr);
  }

  // ---------------------------------------------------------------------------
  const Slave & Master::operator[] (int slaveAddr) const {
    PIMP_D (const Master);

    return * d->slave.at (slaveAddr);
  }

  // ---------------------------------------------------------------------------
  //
  //                         Master::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Master::Private::Private (Master * q, Net net, const std::string & connection,
                            const std::string & settings) :
    Device::Private (q, net, connection, settings) {}

  // ---------------------------------------------------------------------------
  Master::Private::~Private() = default;

}
/* ========================================================================== */
