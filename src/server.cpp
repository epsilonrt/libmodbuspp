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
#include <fstream>
#include <iostream> // for debug
#include <sstream>
#ifdef _WIN32
# include <winsock2.h>
# if defined(SD_BOTH) && ! defined(SHUT_RDWR)
#   define SHUT_RDWR SD_BOTH
# endif
#else
# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
#endif
#include <mutex>
#include "server_p.h"
#include "config.h"

using json = nlohmann::json;

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         Server Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Server::Server (std::unique_ptr<Server::Private> &&dd) : Device (std::move(dd)) {}

  // ---------------------------------------------------------------------------
  Server::Server () :
    Device (std::unique_ptr<Private>(new Private (this))) {}

  // ---------------------------------------------------------------------------
  Server::Server (Net net, const std::string & connection,
                  const std::string & settings) : Server () {
    PIMP_D (Server);

    d->setBackend (net, connection, settings);
  }

  // ---------------------------------------------------------------------------
  Server::Server (const std::string & jsonfile,
                  const std::string & key) : Server () {
    PIMP_D (Server);

    d->setConfigFromFile (jsonfile, key);
  }

  // ---------------------------------------------------------------------------
  Server::~Server() {

    // std::cout << "-- ~Server --" << std::endl;
    terminate();
  }

  // ---------------------------------------------------------------------------
  void Server::close() {

    if (isOpen()) {

      terminate();
      Device::close();
    }
  }

  // ---------------------------------------------------------------------------
  int Server::poll (long timeout) {
    if(isRunning() || not isOpen()){
        // cann not proceed if the `loop()` is running
        // OR connection is not established
        return -1;
    }

    PIMP_D (Server);
    return d->poll(timeout);

  }

  // ---------------------------------------------------------------------------
  bool Server::hasSlave (int slaveAddr) const {
    PIMP_D (const Server);

    return d->slave.count (slaveAddr) > 0;
  }

  // ---------------------------------------------------------------------------
  BufferedSlave & Server::addSlave (int slaveAddr, Device * master) {
    PIMP_D (Server);

    if (isOpen()) {

      throw std::logic_error ("Unable to add slave when open !");
    }
    return *d->addSlave (slaveAddr, master);
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

    return hasSlave (i) ? d->slave.at (i).get() : nullptr;
  }

  // ---------------------------------------------------------------------------
  const BufferedSlave * Server::slavePtr (int slaveAddr) const {
    PIMP_D (const Server);
    int i = d->defaultSlave (slaveAddr);

    return hasSlave (i) ? d->slave.at (i).get() : nullptr;
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
    PIMP_D (Server);

    if (d->listen_sock != -1) {
      ::shutdown (d->listen_sock, SHUT_RDWR);
      ::close(d->listen_sock);
    }

    if (isRunning()) {

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
  const std::map <int, std::shared_ptr<BufferedSlave>> & Server::slaves() const {
    PIMP_D (const Server);

    return d->slave;
  }

  // ---------------------------------------------------------------------------
  Message::Callback Server::messageCallback() const {
    PIMP_D (const Server);

    return d->messageCB;
  }

  // ---------------------------------------------------------------------------
  void Server::setMessageCallback (Message::Callback cb) {
    PIMP_D (Server);

    d->messageCB = cb;
  }

  std::vector<pollfd> Server::fds() {
      PIMP_D(Server);

      return d->all_pollfds;
  }

  // ---------------------------------------------------------------------------
  //
  //                         Server::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Server::Private::Private (Server * q) :
    Device::Private (q), listen_sock (-1), req (0) {
      all_pollfds.reserve(MAX_CONNECTIONS +1);
  }


  // ---------------------------------------------------------------------------
  // virtual
  void Server::Private::setConfig (const nlohmann::json & config) {
    PIMP_Q (Server);

    Json::setConfig (q, config);
  }

  // ---------------------------------------------------------------------------
  // virtual
  void Server::Private::setBackend (Net net, const std::string & connection,
                                    const std::string & settings) {

    Device::Private::setBackend (net, connection, settings);
#if MODBUSPP_HAVE_SERIAL_MULTI_SLAVES
    if (net == Rtu || net == Ascii) {
      modbus_serial_set_recv_filter (ctx(), FALSE);
    }
#endif
  }

  // ---------------------------------------------------------------------------
  BufferedSlave * Server::Private::addSlave (int slaveAddr, Device * master) {

    if (modbus_set_slave (ctx(), slaveAddr) != 0) {
      std::ostringstream oss;

      oss << "Unable to add slave[" << slaveAddr << "]\n" << lastError();
      throw std::invalid_argument (oss.str());
    }

    std::shared_ptr<BufferedSlave> s;

    if (slave.count (slaveAddr)) {

      s = slave[slaveAddr];
      s->setDevice (master);
    }
    else {

      s = std::make_shared<BufferedSlave> (slaveAddr, master);
      slave[slaveAddr] = s;
    }
    return s.get();
  }

  // ---------------------------------------------------------------------------
  bool Server::Private::open() {
    bool isOk = false;

    switch (backend->net()) {

      case Tcp:
        listen_sock = modbus_tcp_pi_listen (ctx(), MAX_CONNECTIONS);
        if ( listen_sock != -1 ) {
          isOk = true;
          all_pollfds.push_back(pollfd{.fd=listen_sock, .events=POLL_IN, .revents=0});
        }
        break;

      case Rtu:
      case Ascii: {
        if ( Device::Private::open() ) {
          isOk = true;
          int listen_sock = modbus_get_socket(ctx());
          all_pollfds.push_back(pollfd{.fd=listen_sock, .events=POLL_IN, .revents=0});
        } else {
            std::cout << "fd after open: " << listen_sock << "\n";
        }
      }
      default:
        break;
    }

    if (isOk && !req) {
      PIMP_Q (Server);

      req = std::make_shared<Request> (*q);
    }

    return isOk;
  }

  // ---------------------------------------------------------------------------
  void Server::Private::close() {
    std::lock_guard<std::mutex> lg (d_guard);
    if (backend->net() == Tcp) {
      if (listen_sock != -1) {
#ifdef _WIN32
        ::closesocket (listen_sock);
#else
        ::close (listen_sock);
#endif
        all_pollfds.clear();
        listen_sock = -1;
      }
    }
    Device::Private::close();
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
        else {

          throw std::runtime_error ("Unable to recovery link !");
        }
      }
    }
    else if (rc > 0) {
      int id = req->slave();

      if (slave.count (id) > 0) {

        if (modbus_set_slave (ctx(), id) == 0) {
          int ret;
          BufferedSlave * slv = slave[id].get();

          // route the message to a possible device to copy its registers to the map.
          ret = slv->readFromDevice (req.get());
          if (ret >= 0) {

            if (slv->beforeReplyCallback()) {
              ret = slv->beforeReplyCallback() (req.get(), q);
              if (ret != 0) { // -1 error, 1 exit, 0 continue
                return ret;
              }
            }

            rc = modbus_reply (ctx(), req->adu(), rc, slv->map());
            if (rc >= 0) {

              if (slv->afterReplyCallback()) {
                ret = slv->afterReplyCallback() (req.get(), q);
                if (ret != 0) { // -1 error, 1 exit, 0 continue
                  return ret;
                }
              }

              // route the message to a possible device to write its registers from map.
              ret = slv->writeToDevice (req.get());
              if (ret < 0) {
                rc = ret;
              }
            }
          }
        }
        rc = 0;
      }
      else {
        if (messageCB) {
          rc = messageCB (req.get(), q);
        }
      }
    }
    return rc;
  }

  int Server::Private::poll(int timeout)
  {
    int eventCount = ::poll(all_pollfds.data(), all_pollfds.size(), timeout);

    if ( eventCount == 0) {
        // there is nothing to process
        return 0;
    }

    if ( eventCount < 0 ) {
        // handle error
        return -1;
    }

    // handle events
    std::vector<pollfd> new_pfds{};

    for ( pollfd& pfd : all_pollfds ) {

      if ( pfd.revents & POLLIN ) {

        if ( backend->net() == Net::Tcp && pfd.fd == listen_sock ) {
          // if there is an event on the 'listening' socket
          // handle incomming connection request aka `connect()`
          // but only Server::Private::MAX_CONNECTIONS
          if ( all_pollfds.size() < MAX_CONNECTIONS ) {
            int new_socket = modbus_tcp_accept(ctx(), &listen_sock);
            if ( new_socket !=  -1 ) {
                new_pfds.push_back(pollfd{
                  .fd=new_socket,
                  .events=POLL_IN,
                  .revents=0});
            }
          }
        } else {
          // handle incomming request
          modbus_set_socket(ctx(), pfd.fd);
          int rc = Server::Private::receive(this);
          if ( rc == -1 ) {
              // if receive fails after a successfull poll
              // probably the connection is broken
              ::close(pfd.fd);
              pfd.fd = -1;
              continue;
          }

          task(rc);
        }
      } else if ( pfd.revents & POLLHUP ) {
          ::close(pfd.fd);
          pfd.fd = -1;
      }
    }

    // remove bad file descriptors from watch list
    auto badFds = std::remove_if(all_pollfds.begin(), all_pollfds.end(), [](const pollfd& pfd) {
        return (pfd.revents & (POLLERR|POLLHUP|POLLNVAL)) || (pfd.fd == -1);
    });
    all_pollfds.erase(badFds, all_pollfds.end());

    if ( all_pollfds.empty() ) {
        return -1;
    }

    // add new accepted file descriptors to watch list
    if ( not new_pfds.empty() ) {
      all_pollfds.insert(all_pollfds.end(), new_pfds.begin(), new_pfds.end());
    }

    return 0;
  }

  // ---------------------------------------------------------------------------
  // static
  int Server::Private::receive (Private * d) {
    std::lock_guard<std::mutex> lg (d->d_guard);

    int rc = 0;

    d->req->clear();
    rc = modbus_receive (d->ctx(), d->req->adu());
    if ( rc > 0 ) {
        d->req->setAduSize(rc);
    }

    return rc;
  }

  // ---------------------------------------------------------------------------
  // static
  void * Server::Private::loop (std::future<void> run, Private * d) {
    int rc;

    while (run.wait_for (std::chrono::milliseconds (100)) == std::future_status::timeout) {

      rc = d->receive (d);
      rc = d->task (rc);
    }
  }
  // ---------------------------------------------------------------------------
  //
  //                         Modbus::Json Namespace
  //
  // ---------------------------------------------------------------------------
  namespace Json {

    // -------------------------------------------------------------------------
    void setConfig (Server * srv, const nlohmann::json & j) {

      setConfig (reinterpret_cast<Device *> (srv), j);
      if (j.contains ("slaves")) {

        auto slaves = j["slaves"];
        for (const auto & config : slaves) {

          auto id = config["id"].get<int>();
          BufferedSlave & slv = srv->addSlave (id);
          setConfig (&slv, config);
        }
      }
    }
  }
}

/* ========================================================================== */
