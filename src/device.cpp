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
#include "device_p.h"
#include "config.h"
#include <chrono>
#include <thread>

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                             Device Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Device::Device (Device::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Device::Device (Net net, const std::string & connection,
                  const std::string & settings) :
    d_ptr (new Private (this, net, connection, settings)) {}

  // ---------------------------------------------------------------------------
  Device::~Device() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool
  Device::open () {
    PIMP_D (Device);

    if (!isOpen()) {

      d->isOpen = (modbus_connect (d->ctx()) == 0);
      if (d->isOpen) {
        // avoid that the slave takes the pulse of 40μs created by the
        // driver when opening the port as a start bit.

        std::this_thread::sleep_for (std::chrono::milliseconds (20));
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void
  Device::close() {

    if (isOpen()) {
      PIMP_D (Device);

      modbus_close (d->ctx());
      d->isOpen = false;
    }
  }

  // ---------------------------------------------------------------------------
  int Device::flush() {

    if (isOpen()) {
      PIMP_D (Device);

      return modbus_flush (d->ctx());
    }
    return -1;
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
    PIMP_D (const Device);

    return d->isOpen && d->isConnected();
  }

  // ---------------------------------------------------------------------------
  void Device::setRecoveryLink (bool recovery) {
    PIMP_D (Device);

    d->recoveryLink = recovery;
  }

  // ---------------------------------------------------------------------------
  bool Device::recoveryLink() const {
    PIMP_D (const Device);

    return d->recoveryLink;
  }

  // ---------------------------------------------------------------------------
  NetLayer & Device::backend() const {
    PIMP_D (const Device);

    return *d->backend;
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
  void Device::setResponseTimeout (const Timeout & t) {
    PIMP_D (Device);

    (void) modbus_set_response_timeout (d->ctx(), t.sec(), t.usec());
  }

  // ---------------------------------------------------------------------------
  void Device::setResponseTimeout (const double & t) {

    setResponseTimeout (Timeout (t));
  }

  // ---------------------------------------------------------------------------
  void Device::responseTimeout (Timeout & t) {
    PIMP_D (Device);

    (void) modbus_get_response_timeout (d->ctx(), &t.m_sec, &t.m_usec);
  }

  // ---------------------------------------------------------------------------
  double Device::responseTimeout () {
    Timeout t;

    responseTimeout (t);
    return t.value();
  }

  // ---------------------------------------------------------------------------
  void Device::setByteTimeout (const Timeout & t) {
    PIMP_D (Device);

    (void) modbus_set_byte_timeout (d->ctx(), t.sec(), t.usec());
  }

  // ---------------------------------------------------------------------------
  void Device::setByteTimeout (const double & t) {

    setByteTimeout (Timeout (t));
  }

  // ---------------------------------------------------------------------------
  void Device::byteTimeout (Timeout & t) {
    PIMP_D (Device);

    (void) modbus_get_byte_timeout (d->ctx(), &t.m_sec, &t.m_usec);
  }

  // ---------------------------------------------------------------------------
  double Device::byteTimeout () {
    Timeout t;

    byteTimeout (t);
    return t.value();
  }

  // ---------------------------------------------------------------------------
  bool Device::setDebug (bool debug) {
    PIMP_D (Device);
    
    int rc = modbus_set_debug (d->ctx(), debug ? TRUE : FALSE);
    if (rc == 0) {
      d->debug = debug;
    }
    
    return (rc == 0);
  }

  // ---------------------------------------------------------------------------
  bool Device::debug () const {
    PIMP_D (const Device);
    
    return d->debug;
  }

  // ---------------------------------------------------------------------------
  Net Device::net() const {

    return backend().net();
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
  Device::Private::Private (Device * q, Net net, const std::string & connection,
                            const std::string & settings) :
    q_ptr (q), isOpen (false), backend (0), recoveryLink (false), debug (false) {

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
  Device::Private::~Private() {

    delete backend;
  }

  // ---------------------------------------------------------------------------
  int Device::Private::defaultSlave (int addr) const {

    if (addr < 0) {

      return backend->net() == Rtu ? Broadcast : TcpSlave;
    }
    return addr;
  }

  // ---------------------------------------------------------------------------
  bool
  Device::Private::isConnected() const {

    return (modbus_get_socket (ctx()) >= 0);
  }

}

/* ========================================================================== */
