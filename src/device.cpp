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
#include <modbuspp/rtulayer.h>
#include <modbuspp/tcplayer.h>
#include <modbuspp/message.h>
#include "device_p.h"
#include "config.h"
#include <chrono>
#include <thread>
#include <iostream> // for debug purposes

using json = nlohmann::json;

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                             Device Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Device::Device (Device::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Device::Device () :  d_ptr (new Private (this)) {}

  // ---------------------------------------------------------------------------
  Device::Device (Net net, const std::string & connection,
                  const std::string & settings) : Device () {
    PIMP_D (Device);

    d->setBackend (net, connection, settings);
  }

  // ---------------------------------------------------------------------------
  Device::Device (const std::string & jsonfile,
                  const std::string & key) : Device () {
    PIMP_D (Device);

    d->setConfigFromFile (jsonfile, key);
  }

  // ---------------------------------------------------------------------------
  Device::~Device() {

    // std::cout << "-- ~Device --" << std::endl;
    close();
  }

  // ---------------------------------------------------------------------------
  std::string Device::connection() const {

    return isValid() ? backend().connection() : std::string ("No backend !");
  }

  // ---------------------------------------------------------------------------
  std::string Device::settings() const {

    return isValid() ? backend().settings() : std::string ("Unknown !");
  }

  // ---------------------------------------------------------------------------
  bool Device::open() {

    if (isValid()) {

      if (!isOpen()) {
        PIMP_D (Device);

        d->isOpen = d->open();
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void Device::close() {

    if (isOpen()) {
      PIMP_D (Device);

      d->close();
      d->isOpen = false;
    }
  }

  // ---------------------------------------------------------------------------
  int Device::flush() {
    
    if (isValid()) {
      PIMP_D (Device);

      return modbus_flush (d->ctx());
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  bool
  Device::isOpen() const {
    PIMP_D (const Device);

    return d->isOpen;
  }

  // ---------------------------------------------------------------------------
  bool
  Device::isConnected() const {

    if (isValid()) {
      PIMP_D (const Device);

      return d->isOpen && d->isConnected();
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::isValid() const {
    PIMP_D (const Device);

    return d->backend != 0;
  }

  // ---------------------------------------------------------------------------
  bool Device::setBackend (Net net, const std::string & connection,
                           const std::string & settings) {
    if (!isValid()) {
      PIMP_D (Device);

      d->setBackend (net, connection, settings);
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::setConfig (const std::string & jsonfile,
                          const std::string & key) {
    if (!isValid()) {
      PIMP_D (Device);

      d->setConfigFromFile (jsonfile, key);
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::setRecoveryLink (bool recovery) {
    PIMP_D (Device);

    d->recoveryLink = recovery;
    return true;
  }

  // ---------------------------------------------------------------------------
  bool Device::recoveryLink() const {
    PIMP_D (const Device);

    return d->recoveryLink;
  }

  // ---------------------------------------------------------------------------
  NetLayer & Device::backend() const {

    if (isValid()) {
      PIMP_D (const Device);

      return *d->backend;
    }
    throw std::domain_error ("Error: Unable to return backend !");
  }

  // ---------------------------------------------------------------------------
  RtuLayer & Device::rtu() {

    if (net() == Rtu) {
      PIMP_D (Device);

      return * reinterpret_cast<RtuLayer *> (d->backend);
    }
    throw std::domain_error ("Error: Unable to return RTU layer !");
  }

  // ---------------------------------------------------------------------------
  TcpLayer & Device::tcp() {

    if (net() == Tcp) {
      PIMP_D (Device);

      return * reinterpret_cast<TcpLayer *> (d->backend);
    }
    throw std::domain_error ("Error: Unable to return TCP layer !");
  }

  // ---------------------------------------------------------------------------
  bool Device::setResponseTimeout (const Timeout & t) {

    if (isValid()) {
      PIMP_D (Device);

      return modbus_set_response_timeout (d->ctx(), t.sec(), t.usec()) == 0;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  bool Device::setResponseTimeout (const double & t) {

    return setResponseTimeout (Timeout (t));
  }

  // ---------------------------------------------------------------------------
  bool Device::responseTimeout (Timeout & t) {

    if (isValid()) {
      PIMP_D (Device);

      return modbus_get_response_timeout (d->ctx(), &t.m_sec, &t.m_usec) == 0;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  double Device::responseTimeout () {
    Timeout t;

    responseTimeout (t);
    return t.value();
  }

  // ---------------------------------------------------------------------------
  bool Device::setByteTimeout (const Timeout & t) {

    if (isValid()) {
      PIMP_D (Device);

      return modbus_set_byte_timeout (d->ctx(), t.sec(), t.usec()) == 0;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  bool Device::setByteTimeout (const double & t) {

    return setByteTimeout (Timeout (t));
  }

  // ---------------------------------------------------------------------------
  bool Device::byteTimeout (Timeout & t) {

    if (isValid()) {
      PIMP_D (Device);

      return modbus_get_byte_timeout (d->ctx(), &t.m_sec, &t.m_usec) == 0;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  double Device::byteTimeout () {
    Timeout t;

    byteTimeout (t);
    return t.value();
  }

  // ---------------------------------------------------------------------------
  bool Device::setDebug (bool debug) {

    if (isValid()) {
      PIMP_D (Device);

      if (modbus_set_debug (d->ctx(), debug ? TRUE : FALSE) == 0) {

        d->debug = debug;
      }
      return d->debug == debug;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  bool Device::debug () const {
    PIMP_D (const Device);

    return d->debug;
  }

  // ---------------------------------------------------------------------------
  Net Device::net() const {

    if (isValid()) {

      return backend().net();
    }
    return NoNet;
  }

  /* ---------------------------------------------------------------------------
   * Inspired by msg_send from modbus.c :
   * Copyright © 2001-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
   *
   * SPDX-License-Identifier: LGPL-2.1-or-later
   */
  int Device::sendRawMessage (Message * msg, bool prepareBefore) {

    if (isValid()) {
      PIMP_D (Device);
      int rc;

      if (prepareBefore) {

        if (!backend().prepareToSend (*msg)) {

          errno = EINVAL;
          return -1;
        }
      }

      if (d->debug) {

        msg->print (std::cout, '[', ']');
        std::cout << std::endl;
      }

      do {
        rc = backend().sendRawMessage (msg);

        if (rc == -1) {

          d->printError();
          if (d->recoveryLink && !msg->isResponse()) {
            int saved_errno = errno;
            long timeout = responseTimeout() * 1000;

            if ( (errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {

              d->close ();
              std::this_thread::sleep_for (std::chrono::milliseconds (timeout));
              d->open ();
            }
            else {

              std::this_thread::sleep_for (std::chrono::milliseconds (timeout));
              flush();
            }
            errno = saved_errno;
          }
        }
      }
      while (d->recoveryLink && rc == -1 && !msg->isResponse());

      if (rc > 0 && rc != msg->size()) {

        errno = EMBBADDATA;
        return -1;
      }

      return rc;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  int Device::sendRawMessage (Message & msg, bool prepareBefore) {

    return sendRawMessage (&msg, prepareBefore);
  }

  // ---------------------------------------------------------------------------
  // static
  std::string Device::lastError() {

    return NetLayer::lastError();
  }

  // ---------------------------------------------------------------------------
  //
  //                         Device::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Device::Private::Private (Device * q) :
    q_ptr (q), isOpen (false), backend (0), recoveryLink (false),
    debug (false) {}

  // ---------------------------------------------------------------------------
  Device::Private::~Private() {

    delete backend;
  }

  // ---------------------------------------------------------------------------
  void Device::Private::setConfigFromFile (const std::string & jsonfile,
      const std::string & key) {

    // read a JSON file
    std::ifstream ifs (jsonfile);

    if (ifs.is_open()) {
      json j, config;

      ifs >> j;
      // std::cout << "file > " << j << std::endl; // debug
      if (! key.empty() && j.contains (key)) {

        config = j[key];
      }
      else {

        config = j;
      }
      // std::cout << "config > " << config << std::endl; // debug
      setConfig (config);
    }
    else {

      throw std::system_error (errno, std::system_category(),
                               "failed to open " + jsonfile);
    }
  }

  // ---------------------------------------------------------------------------
  // virtual
  void Device::Private::setConfig (const nlohmann::json & config) {
    PIMP_Q (Device);

    Json::setConfig (q, config);
  }

  // ---------------------------------------------------------------------------
  // virtual
  void Device::Private::setBackend (Net net, const std::string & connection,
                                    const std::string & settings) {

    switch (net) {

      case Tcp:
        backend = new TcpLayer (connection, settings);
        break;

      case Rtu:
        backend = new RtuLayer (connection, settings);
        break;

      default:
        throw std::invalid_argument (
          "Error: Unable to create Modbus Device for this net !");
        break;
    }
  }

  // ---------------------------------------------------------------------------
  bool
  Device::Private::open () {

    if (modbus_connect (ctx()) == 0) {
      // avoid that the slave takes the pulse of 40μs created by the
      // driver when opening the port as a start bit.

      std::this_thread::sleep_for (std::chrono::milliseconds (20));
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void
  Device::Private::close() {

    modbus_close (ctx());
  }

  // ---------------------------------------------------------------------------
  int Device::Private::defaultSlave (int addr) const {

    if (addr < 0 && backend != 0) {

      return backend->net() == Rtu ? Broadcast : TcpSlave;
    }
    return addr;
  }

  // ---------------------------------------------------------------------------
  bool
  Device::Private::isConnected() const {

    return (modbus_get_socket (ctx()) >= 0);
  }

  // ---------------------------------------------------------------------------
  void Device::Private::printError (const char * what) const {

    if (debug) {

      std::cerr << "ERROR " << modbus_strerror (errno);
      if (what) {

        std::cerr << ": " << what;
      }
      std::cerr << std::endl;
    }
  }

  // ---------------------------------------------------------------------------
  //
  //                        Modbus::Json Namespace
  //
  // ---------------------------------------------------------------------------
  namespace Json {

    // -------------------------------------------------------------------------
    void setConfig (Device * dev, const nlohmann::json & config) {
      auto connection =  config["connection"].get<std::string>();
      auto settings =  config["settings"].get<std::string>();
      auto net = config["mode"].get<Net>();

      if (dev->setBackend (net, connection, settings)) {

        if (config.contains ("recovery-link")) {

          auto b = config["recovery-link"].get<bool>();
          dev->setRecoveryLink (b);
        }
        if (config.contains ("debug")) {

          auto b = config["debug"].get<bool>();
          dev->setDebug (b);
        }
        if (config.contains ("response-timeout")) {

          auto d = config["response-timeout"].get<double>();
          d /= 1000;
          dev->setResponseTimeout (d);
        }
        if (config.contains ("byte-timeout")) {

          auto d = config["byte-timeout"].get<double>();
          d /= 1000;
          dev->setByteTimeout (d);
        }
        if (config.contains ("rtu") && net == Rtu) {
          auto rtu = config["rtu"];

          if (rtu.contains ("mode")) {
            auto m = rtu["mode"].get<SerialMode>();
            dev->rtu().setSerialMode (m);
          }
          if (rtu.contains ("rts")) {
            auto r = rtu["rts"].get<SerialRts>();
            dev->rtu().setRts (r);
          }
          if (rtu.contains ("rts-delay")) {
            auto r = rtu["rts-delay"].get<int>();
            dev->rtu().setRtsDelay (r);
          }
        }
      }
    }
  }
}

/* ========================================================================== */
