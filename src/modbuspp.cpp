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
#include "modbuspp_p.h"
#include "config.h"
#include <chrono>
#include <thread>
#include <stdexcept>

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

    if (!isOpen() && !isNull()) {

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
  bool Device::flush() {

    if (isOpen()) {
      PIMP_D (Device);

      return (modbus_flush (d->ctx) >= 0);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  Device::isOpen() const {
    PIMP_D (const Device);

    return d->isOpen;
  }

  // ---------------------------------------------------------------------------
  bool Device::isNull() const {
    PIMP_D (const Device);

    return d->ctx == 0;
  }

  // ---------------------------------------------------------------------------
  int Device::slave() const {
    PIMP_D (const Device);

    return d->slave;
  }

  // ---------------------------------------------------------------------------
  bool Device::setSlave (int id) {
    PIMP_D (Device);

    if (!isNull()) {

      if (modbus_set_slave (d->ctx, id) == 0) {

        d->slave = id;
        return true;
      }
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
  bool Device::setResponseTimeout (const Timeout & t) {
    PIMP_D (Device);

    if (!isNull()) {

      return (modbus_set_response_timeout (d->ctx, t.sec, t.usec) == 0);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::responseTimeout (Timeout & t) {
    PIMP_D (Device);

    if (!isNull()) {

      return (modbus_get_response_timeout (d->ctx, &t.sec, &t.usec) == 0);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::setByteTimeout (const Timeout & t) {
    PIMP_D (Device);

    if (!isNull()) {

      return (modbus_set_byte_timeout (d->ctx, t.sec, t.usec) == 0);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::byteTimeout (Timeout & t) {
    PIMP_D (Device);

    if (!isNull()) {

      return (modbus_get_byte_timeout (d->ctx, &t.sec, &t.usec) == 0);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::setDebug (bool debug) {
    PIMP_D (Device);

    if (!isNull()) {

      return (modbus_set_debug (d->ctx, debug ? TRUE : FALSE) == 0);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // static
  std::string lastError() {

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

  // ---------------------------------------------------------------------------
  //
  //                             Master Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Master::Master (Master::Private &dd) : Device (dd) {

  }

  // ---------------------------------------------------------------------------
  Master::Master (Net net, const std::string & connection,
                  const std::string & settings) :
    Device (*new Private (this, net, connection, settings)) {}

  // ---------------------------------------------------------------------------
  Master::~Master() = default;

  // ---------------------------------------------------------------------------
  int Master::readDiscreteInputs (int addr, bool * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_read_input_bits (d->ctx,
                                     d->address (addr), nb, (uint8_t *) dest);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readCoils (int addr, bool * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_read_bits (d->ctx,
                               d->address (addr), nb, (uint8_t *) dest);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readInputRegisters (int addr, uint16_t * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_read_input_registers (d->ctx,
                                          d->address (addr), nb, dest);
    }

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readRegisters (int addr, uint16_t * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_read_registers (d->ctx,
                                    d->address (addr), nb, dest);
    }

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeCoil (int addr, bool value) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_bit (d->ctx,
                               d->address (addr), (uint8_t) value);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeCoils (int addr, const bool * src, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_bits (d->ctx,
                                d->address (addr), nb, (const uint8_t *) src);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeRegister (int addr, uint16_t value) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_register (d->ctx,
                                    d->address (addr), value);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeRegisters (int addr, const uint16_t * src, int nb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_registers (d->ctx, d->address (addr), nb, src);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeReadRegisters (int waddr, const uint16_t * src, int wnb,
                                  int raddr, uint16_t * dest, int rnb) {

    if (isOpen()) {
      PIMP_D (Master);

      return modbus_write_and_read_registers (d->ctx,
                                              d->address (waddr), wnb, src,
                                              d->address (raddr), rnb, dest);
    }
    return -1;
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

  // ---------------------------------------------------------------------------
  //
  //                          NetLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  NetLayer::NetLayer (NetLayer::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  //
  //                         RtuLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  RtuLayer::RtuLayer (RtuLayer::Private &dd) : NetLayer (dd) {}

  // ---------------------------------------------------------------------------
  SerialMode RtuLayer::serialMode() {
    PIMP_D (RtuLayer);

    int m = modbus_rtu_get_serial_mode (d->ctx);
    if (m != -1) {
      return static_cast<SerialMode> (m);
    }
    return UnknownMode;
  }

  // ---------------------------------------------------------------------------
  bool RtuLayer::setSerialMode (SerialMode mode) {
    PIMP_D (RtuLayer);

    return (modbus_rtu_set_serial_mode (d->ctx, static_cast<int> (mode)) != -1);
  }

  // ---------------------------------------------------------------------------
  SerialRts RtuLayer::rts() {
    PIMP_D (RtuLayer);

    int r = modbus_rtu_get_rts (d->ctx);
    if (r != -1) {

      return static_cast<SerialRts> (r);
    }
    return UnknownRts;
  }

  // ---------------------------------------------------------------------------
  bool RtuLayer::setRts (SerialRts r) {
    PIMP_D (RtuLayer);

    return (modbus_rtu_set_rts (d->ctx, static_cast<int> (r)) != -1);
  }

  // ---------------------------------------------------------------------------
  int RtuLayer::rtsDelay() {
    PIMP_D (RtuLayer);

    return modbus_rtu_get_rts_delay (d->ctx);
  }

  // ---------------------------------------------------------------------------
  bool RtuLayer::setRtsDelay (int delay) {
    PIMP_D (RtuLayer);

    return (modbus_rtu_set_rts_delay (d->ctx, delay) != -1);
  }

  // ---------------------------------------------------------------------------
  const std::string & RtuLayer::port() const {
    PIMP_D (const RtuLayer);

    return d->port;
  }

  // ---------------------------------------------------------------------------
  int RtuLayer::baud() const {
    PIMP_D (const RtuLayer);

    return baud (d->settings);
  }

  // ---------------------------------------------------------------------------
  char RtuLayer::parity() const {
    PIMP_D (const RtuLayer);

    return parity (d->settings);
  }

  // ---------------------------------------------------------------------------
  int RtuLayer::stop() const {
    PIMP_D (const RtuLayer);

    return stop (d->settings);
  }

  // ---------------------------------------------------------------------------
  // static
  int RtuLayer::baud (const std::string & settings) {
    int b;
    try {
      b = std::stoi (settings);
    }
    catch (...) {
      b = 19200;
    }
    return b;
  }

  // ---------------------------------------------------------------------------
  // static
  char RtuLayer::parity (const std::string & settings) {
    char p = 'N';
    size_t s = settings.length();

    if (s >= 2) {
      char c = settings[s - 2];
      if ( (c == 'E') || (c == 'O')) {
        return c;
      }
    }
    return p;
  }

  // ---------------------------------------------------------------------------
  // static
  int RtuLayer::stop (const std::string & settings) {

    if (parity (settings) == 'N') {

      return 2;
    }
    return 1;
  }

  // ---------------------------------------------------------------------------
  //
  //                         TcpLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  TcpLayer::TcpLayer (TcpLayer::Private &dd) : NetLayer (dd) {}

  // ---------------------------------------------------------------------------
  const std::string & TcpLayer::node() const {
    PIMP_D (const TcpLayer);

    return d->host;
  }

  // ---------------------------------------------------------------------------
  const std::string & TcpLayer::service() const {
    PIMP_D (const TcpLayer);

    return d->service;
  }

}

/* ========================================================================== */
