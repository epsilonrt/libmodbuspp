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
    d_ptr (new Private (this, net, connection, settings)) {

  }

  // ---------------------------------------------------------------------------
  Device::Device (const Device & other) :
    d_ptr (new Private (*other.d_ptr)) {}

  // ---------------------------------------------------------------------------
  void Device::swap (Device &other) {

    d_ptr.swap (other.d_ptr);
  }

  // ---------------------------------------------------------------------------
  Device& Device::operator= (const Device &other) {

    Device (other).swap (*this);
    return *this;
  }

  // ---------------------------------------------------------------------------
  Device::~Device() {

    close();
    modbus_free (d_ptr->ctx);
  }

  // ---------------------------------------------------------------------------
  void Device::setPduAdressing (bool pdu) {
    PIMP_D (Device);

    d->pduAddressing = pdu;
  }

  // ---------------------------------------------------------------------------
  bool  Device::pduAdressing() const {
    PIMP_D (const Device);

    return d->pduAddressing;
  }

  // ---------------------------------------------------------------------------
  bool
  Device::open () {
    PIMP_D (Device);

    if (!isOpen()) {

      d->isOpen = (modbus_connect (d->ctx) == 0);
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

      modbus_close (d->ctx);
      d->isOpen = false;
    }
  }

  // ---------------------------------------------------------------------------
  int Device::flush() {

    if (isOpen()) {
      PIMP_D (Device);

      return modbus_flush (d->ctx);
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
  int Device::readDiscreteInputs (int addr, bool * dest, int nb) {

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Device::readCoils (int addr, bool * dest, int nb) {

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Device::readInputRegisters (int addr, uint16_t * dest, int nb) {

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Device::readRegisters (int addr, uint16_t * dest, int nb) {

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Device::writeCoil (int addr, bool value) {

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Device::writeCoils (int addr, const bool * src, int nb) {

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Device::writeRegister (int addr, uint16_t value) {

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Device::writeRegisters (int addr, const uint16_t * src, int nb) {

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Device::slave() const {
    PIMP_D (const Device);

    return d->slave;
  }

  // ---------------------------------------------------------------------------
  bool Device::setSlave (int id) {
    PIMP_D (Device);

    if (modbus_set_slave (d->ctx, id) == 0) {

      d->slave = id;
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  Net Device::net() const {
    PIMP_D (const Device);

    if (d->rtu) {

      return Rtu;
    }
    else if (d->tcp) {

      return Tcp;
    }
    return NoNet;
  }

  // ---------------------------------------------------------------------------
  RtuLayer & Device::rtu() {

    if (net() == Rtu) {
      PIMP_D (Device);

      return *d->rtu;
    }
    throw std::domain_error ("Error: Unable to return RTU layer !");
  }

  // ---------------------------------------------------------------------------
  TcpLayer & Device::tcp() {

    if (net() == Tcp) {
      PIMP_D (Device);

      return *d->tcp;
    }
    throw std::domain_error ("Error: Unable to return TCP layer !");
  }

  // ---------------------------------------------------------------------------
  void Device::setResponseTimeout (const Timeout & t) {
    PIMP_D (Device);

    (void) modbus_set_response_timeout (d->ctx, t.sec(), t.usec());
  }

  // ---------------------------------------------------------------------------
  void Device::setResponseTimeout (const double & t) {

    setResponseTimeout (Timeout (t));
  }

  // ---------------------------------------------------------------------------
  void Device::responseTimeout (Timeout & t) {
    PIMP_D (Device);

    (void) modbus_get_response_timeout (d->ctx, &t.m_sec, &t.m_usec);
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

    (void) modbus_set_byte_timeout (d->ctx, t.sec(), t.usec());
  }

  // ---------------------------------------------------------------------------
  void Device::setByteTimeout (const double & t) {

    setByteTimeout (Timeout (t));
  }

  // ---------------------------------------------------------------------------
  void Device::byteTimeout (Timeout & t) {
    PIMP_D (Device);

    (void) modbus_get_byte_timeout (d->ctx, &t.m_sec, &t.m_usec);
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

    return (modbus_set_debug (d->ctx, debug ? TRUE : FALSE) == 0);
  }

  // ---------------------------------------------------------------------------
  // static
  std::string Device::lastError() {

    return modbus_strerror (errno);
  }

  // ---------------------------------------------------------------------------
  //
  //                         Device::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Device::Private::Private (Device * q, Net net, const std::string & connection,
                            const std::string & settings) :
    q_ptr (q), ctx (0), isOpen (false), pduAddressing (false),
    rtu (0), tcp (0), slave (0) {

    switch (net) {

      case Tcp:
        ctx = modbus_new_tcp_pi (connection.c_str(), settings.c_str());
        if (ctx) {
          tcp = new TcpLayer (*new TcpLayer::Private (ctx, connection, settings));
        }
        else {

          throw std::invalid_argument (
            "Error: Unable to create TCP Modbus Device(" +
            connection + "," + settings + ")");
        }
        slave = TcpSlave;
        break;

      case Rtu:
        // RTU MUST BE 8-bits
        ctx = modbus_new_rtu (connection.c_str(), RtuLayer::baud (settings),
                              RtuLayer::parity (settings), 8,
                              RtuLayer::stop (settings));
        if (ctx) {

          rtu = new RtuLayer (*new RtuLayer::Private (ctx, connection, settings));
        }
        else {

          throw std::invalid_argument (
            "Error: Unable to create RTU Modbus Device("
            + connection + "," + settings + ")");
        }
        break;

      default:
        throw std::invalid_argument (
          "Error: Unable to create Modbus Device for this net !");
        break;
    }

  }

  // ---------------------------------------------------------------------------
  Device::Private::~Private() {

    delete rtu;
    delete tcp;
  }

  // ---------------------------------------------------------------------------
  int Device::Private::address (int addr) {

    return addr - (pduAddressing ? 0 : 1);
  }
}

/* ========================================================================== */
