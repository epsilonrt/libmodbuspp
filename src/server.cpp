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

    d->req = new Request (*this);
  }

  // ---------------------------------------------------------------------------
  Server::~Server() {

    terminate();
  }


  // ---------------------------------------------------------------------------
  bool Server::open() {

    if (!isOpen()) {
      PIMP_D (Server);

      return d->open();
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void Server::close() {

    if (isOpen()) {
      PIMP_D (Server);

      terminate();
      d->close();
    }
  }

  // ---------------------------------------------------------------------------
  int Server::poll (long timeout) {

    if (!isRunning() && isOpen()) {
      PIMP_D (Server);

      if (!d->receiveTask.valid()) {
        // starts the receiving thread
        d->receiveTask = std::async (std::launch::async, Server::Private::receive, d);
      }

      if (d->receiveTask.wait_for (std::chrono::milliseconds (timeout)) == std::future_status::ready) {

        // message received ?
        int rc = d->receiveTask.get();
        return d->task (rc);
      }
      return 0;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  bool Server::hasSlave (int slaveAddr) const {
    PIMP_D (const Server);

    return d->slave.count (slaveAddr) > 0;
  }

  // ---------------------------------------------------------------------------
  BufferedSlave & Server::addSlave (int slaveAddr, Device * master) {
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
  bool Server::run () {

    if (!isRunning() && isOpen()) {
      PIMP_D (Server);

      // Fetch std::future object associated with promise
      std::future<void> running = d->stopDaemon.get_future();

      // Starting Thread & move the future object in lambda function by reference
      d->daemon = std::thread (&Private::loop, std::move (running), d);
    }
    return isRunning();
  }

  // ---------------------------------------------------------------------------
  void Server::terminate () {

    if (isRunning()) {
      PIMP_D (Server);

      // Set the value in promise
      d->stopDaemon.set_value();
      // Wait for thread to join
      d->daemon.join();
    }
  }

  // ---------------------------------------------------------------------------
  bool Server::isRunning() const {
    PIMP_D (const Server);

    return d->daemon.joinable();
  }

  // ---------------------------------------------------------------------------
  //
  //                         Server::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Server::Private::Private (Server * q, Net net, const std::string & connection,
                            const std::string & settings) :
    Device::Private (q, net, connection, settings), sock (-1), req (0) {}

  // ---------------------------------------------------------------------------
  Server::Private::~Private() {

    delete req;
  }

  // ---------------------------------------------------------------------------
  bool Server::Private::open() {
    PIMP_Q (Server);

    switch (backend->net()) {

      case Tcp:
        sock = modbus_tcp_pi_listen (ctx(), 1);
        isOpen = (sock != -1);
        break;

      case Rtu:
        return q->Device::open();

      default:
        break;
    }

    return isOpen;
  }

  // ---------------------------------------------------------------------------
  void Server::Private::close() {
    PIMP_Q (Server);

    if (backend->net() == Tcp) {

      if (sock != -1) {
        ::close (sock);
        sock = -1;
      }
    }
    q->Device::close();
  }

  // ---------------------------------------------------------------------------
  int Server::Private::task (int rc) {
    PIMP_Q (Server);

    if (rc == -1 && errno != EMBBADCRC) {

      if (q->recoveryLink()) {
        close();
        if (open()) {

          errno = 0;
          rc = 0;
        }
      }
    }
    else if (rc > 0) {
      int id = req->slave();

      if (slave.count (id) > 0) {

        if (modbus_set_slave (ctx(), id) == 0) {
          int ret;
          BufferedSlave * slv = slave[id];

          req->adu().resize (rc);

          // route the message to a possible device to copy its registers to the map.
          ret = slv->readFromDevice (req);
          if (ret >= 0) {

            if (slv->beforeReplyCallback()) {
              ret = slv->beforeReplyCallback() (*req, q);
              if (ret != 0) { // -1 error, 1 exit, 0 continue
                return ret;
              }
            }

            rc = modbus_reply (ctx(), req->adu().data(), rc, slv->map());
            if (rc >= 0) {

              if (slv->afterReplyCallback()) {
                ret = slv->afterReplyCallback() (*req, q);
                if (ret != 0) { // -1 error, 1 exit, 0 continue
                  return ret;
                }
              }

              // route the message to a possible device to write its registers from map.
              ret = slv->writeToDevice (req);
              if (ret < 0) {
                rc = ret;
              }
            }
          }
        }
      }
    }
    return rc;
  }

  // ---------------------------------------------------------------------------
  // static
  int Server::Private::receive (Private * d) {
    int rc;
    if ( (d->backend->net() == Tcp) && !d->isConnected()) {

      // accept blocking call !
      if (modbus_tcp_pi_accept (d->ctx(), &d->sock) < 0) {

        return -1;
      }
    }
    rc = modbus_receive (d->ctx(), d->req->adu().data());
    return rc;
  }

  // ---------------------------------------------------------------------------
  // static
  void * Server::Private::loop (std::future<void> run, Private * d) {
    int rc;

    while (run.wait_for (std::chrono::milliseconds (1)) == std::future_status::timeout) {

      rc = d->receive (d);
      rc = d->task (rc);
    }
  }
}

/* ========================================================================== */
