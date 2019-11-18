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
#ifdef _WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif
#include <unistd.h>

#include "server_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         Server Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Server::Server (Server::Private &dd) : Device (dd) {}

  // ---------------------------------------------------------------------------
  Server::Server (Net net, const std::string & connection,
                  const std::string & settings) :
    Device (*new Private (this, net, connection, settings)) {
    PIMP_D (Server);

    d->msg = new Message (*this);
  }

  // ---------------------------------------------------------------------------
  Server::~Server() = default;

  // ---------------------------------------------------------------------------
  bool Server::open() {
    PIMP_D (Server);

    switch (net()) {

      case Tcp:
        d->sock = modbus_tcp_pi_listen (d->ctx(), 1);
        d->isOpen = (d->sock != -1);
        break;

      case Rtu:
        return Device::open();

      default:
        break;
    }

    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void Server::close() {
    PIMP_D (Server);

    if (isOpen()) {

      if (net() == Tcp) {

        if (d->sock != -1) {
          ::close (d->sock);
          d->sock = -1;
        }
      }
      Device::close();
    }
  }

  // ---------------------------------------------------------------------------
  int Server::task() {

    if (isOpen()) {
      PIMP_D (Server);
      int rc;

      if (!isConnected() && (net() == Tcp)) {

        modbus_tcp_pi_accept (d->ctx(), &d->sock);
      }

      rc = modbus_receive (d->ctx(), d->msg->adu().data());
      if (rc == -1 && errno != EMBBADCRC) {

        switch (errno) {
          case ECONNRESET:
            if (recoveryLink()) {
              close();
              open();
              errno = 0;
            }
            break;
          default:
            break;
        }
      }
      else if (rc > 0) {
        int id = d->msg->slaveId();

        if (hasSlave (id)) {

          if (modbus_set_slave (d->ctx(), id) == 0) {
            BufferedSlave * slv = d->slave[id];

            if (slv->beforeReplyCallback()) {
              rc = slv->beforeReplyCallback() (*d->msg, this);
              if (rc != 0) {
                return rc;
              }
            }
            // route the message to a possible device to copy its registers to the map.
            slv->readFromDevice (d->msg); 
            rc = modbus_reply (d->ctx(), d->msg->adu().data(), rc, slv->map());
            if (rc >= 0) {
              if (slv->afterReplyCallback()) {
                rc = slv->afterReplyCallback() (*d->msg, this);
                if (rc != 0) {
                  return rc;
                }
              }
              // route the message to a possible device to write its registers from map.
              slv->writeToDevice (d->msg);
            }
          }
        }
      }

      return rc;
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  bool Server::hasSlave (int slaveAddr) const {
    PIMP_D (const Server);

    return d->slave.count (slaveAddr) > 0;
  }

  // ---------------------------------------------------------------------------
  BufferedSlave & Server::addSlave (int slaveAddr, Master * master) {
    PIMP_D (Server);

    if (modbus_set_slave (d->ctx(), slaveAddr) != 0) {
      std::ostringstream oss;

      oss << "Error: Unable to add slave[" << slaveAddr << "]\n" << lastError();
      throw std::invalid_argument (oss.str());
    }

    if (hasSlave (slaveAddr)) {

      return slave (slaveAddr);
    }
    BufferedSlave * s = new BufferedSlave (slaveAddr, master);
    d->slave[slaveAddr] = s;
    return *s;
  }

  // ---------------------------------------------------------------------------
  BufferedSlave & Server::slave (int slaveAddr) {
    PIMP_D (Server);

    return *d->slave.at (d->defaultSlave (slaveAddr));
  }

  // ---------------------------------------------------------------------------
  const BufferedSlave & Server::slave (int slaveAddr) const {
    PIMP_D (const Server);

    return *d->slave.at (d->defaultSlave (slaveAddr));
  }

  // ---------------------------------------------------------------------------
  BufferedSlave * Server::slavePtr (int slaveAddr) {
    PIMP_D (Server);
    int i = d->defaultSlave (slaveAddr);

    return hasSlave (i) ? d->slave.at (i) : 0;
  }

  // ---------------------------------------------------------------------------
  const BufferedSlave * Server::slavePtr (int slaveAddr) const {
    PIMP_D (const Server);
    int i = d->defaultSlave (slaveAddr);

    return hasSlave (i) ? d->slave.at (i) : 0;
  }

  // ---------------------------------------------------------------------------
  BufferedSlave & Server::operator[] (int slaveAddr) {
    PIMP_D (Server);

    return * d->slave.at (slaveAddr);
  }

  // ---------------------------------------------------------------------------
  const BufferedSlave & Server::operator[] (int slaveAddr) const {
    PIMP_D (const Server);

    return * d->slave.at (slaveAddr);
  }

  // ---------------------------------------------------------------------------
  //
  //                         Server::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Server::Private::Private (Server * q, Net net, const std::string & connection,
                            const std::string & settings) :
    Device::Private (q, net, connection, settings), sock (-1), msg (0) {}

  // ---------------------------------------------------------------------------
  Server::Private::~Private() {

    delete msg;
  }
}

/* ========================================================================== */
