/* Copyright © 2018 Pascal JEAN, All rights reserved.
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

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                             Device Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Device::Device (Device::Private &dd) : d_ptr (&dd) {

  }

  // ---------------------------------------------------------------------------
  Device::Device (const DataLinkLayer & sublayer) :
    d_ptr (new Private (this, sublayer))  {

  }

  // ---------------------------------------------------------------------------
  Device::Device (const Device & other) :
    d_ptr (new Private (*other.d_ptr)) {

  }

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
    if (!isNull()) {

      modbus_free (d_ptr->ctx);
    }
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

    if (!isOpen() && !isNull()) {
      PIMP_D (Device);

      d->isOpen = (modbus_connect (d->ctx) == 0);
      if (!d->isOpen) {

        d->error = modbus_strerror (errno);
      }
      else {
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

      if (modbus_flush (d->ctx) >= 0) {
        return true;
      }
      d->error = modbus_strerror (errno);
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
  const std::string & Device::error() const {
    PIMP_D (const Device);

    return d->error;
  }

  // ---------------------------------------------------------------------------
  const DataLinkLayer & Device::dataLinkLayer() const {
    PIMP_D (const Device);

    return d->dll;
  }

  // ---------------------------------------------------------------------------
  bool Device::isNull() const {
    PIMP_D (const Device);

    return (d->ctx == 0);
  }

  // ---------------------------------------------------------------------------
  bool Device::setResponseTimeout (const Timeout & t) {

    if (!isNull()) {
      PIMP_D (Device);

      if (modbus_set_response_timeout (d->ctx, t.sec, t.usec) == 0) {
        return true;
      }
      d->error = modbus_strerror (errno);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::responseTimeout (Timeout & t) {

    if (!isNull()) {
      PIMP_D (Device);

      if (modbus_get_response_timeout (d->ctx, &t.sec, &t.usec) == 0) {
        return true;
      }
      d->error = modbus_strerror (errno);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::setByteTimeout (const Timeout & t) {

    if (!isNull()) {
      PIMP_D (Device);

      if (modbus_set_byte_timeout (d->ctx, t.sec, t.usec) == 0) {
        return true;
      }
      d->error = modbus_strerror (errno);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::byteTimeout (Timeout & t) {

    if (!isNull()) {
      PIMP_D (Device);

      if (modbus_get_byte_timeout (d->ctx, &t.sec, &t.usec) == 0) {
        return true;
      }
      d->error = modbus_strerror (errno);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Device::setDebug (bool debug) {

    if (!isNull()) {
      PIMP_D (Device);

      if (modbus_set_debug (d->ctx, debug ? TRUE : FALSE) == 0) {
        return true;
      }
      d->error = modbus_strerror (errno);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  SerialMode Device::serialMode() {

    if (!isNull()) {
      PIMP_D (Device);

      if (d->dll.type() == DataLinkLayer::Rtu) {
        int m = modbus_rtu_get_serial_mode (d->ctx);
        if (m != -1) {
          return static_cast<SerialMode> (m);
        }
        else {

          d->error = modbus_strerror (errno);
        }
      }
    }
    return UnknownMode;
  }

  // ---------------------------------------------------------------------------
  bool Device::setSerialMode (SerialMode mode) {

    if (!isNull()) {
      PIMP_D (Device);

      if (d->dll.type() == DataLinkLayer::Rtu) {
        if (modbus_rtu_set_serial_mode (d->ctx, static_cast<int> (mode)) != -1) {

          return true;
        }
        else {

          d->error = modbus_strerror (errno);
        }
      }
    }
    return false;
  }
  
  // ---------------------------------------------------------------------------
  SerialRts Device::rts() {

    if (!isNull()) {
      PIMP_D (Device);

      if (d->dll.type() == DataLinkLayer::Rtu) {
        int r = modbus_rtu_get_rts (d->ctx);
        if (r != -1) {
          
          return static_cast<SerialRts> (r);
        }
        else {

          d->error = modbus_strerror (errno);
        }
      }
    }
    return UnknownRts;
  }
  
  // ---------------------------------------------------------------------------
  bool Device::setRts (SerialRts r) {

    if (!isNull()) {
      PIMP_D (Device);

      if (d->dll.type() == DataLinkLayer::Rtu) {
        if (modbus_rtu_set_rts (d->ctx, static_cast<int> (r)) != -1) {

          return true;
        }
        else {

          d->error = modbus_strerror (errno);
        }
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  //
  //                         Device::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Device::Private::Private (Device * q, const DataLinkLayer & sublayer) :
    q_ptr (q), dll (sublayer), ctx (0), isOpen (false) {

    switch (sublayer.type()) {

      case DataLinkLayer::Tcp: {
        const TcpLayer & tcp (sublayer);

        ctx = modbus_new_tcp_pi (tcp.node(), tcp.service());
      }
      break;

      case DataLinkLayer::Rtu: {
        const RtuLayer & rtu (sublayer);

        // RTU MUST BE 8-bits
        ctx = modbus_new_rtu (rtu.device(), rtu.baud(), rtu.parity(), 8, rtu.stop());
      }
      break;

      default:
        break;
    }

    if (ctx) {

      dll = sublayer;
    }
    else {

      error = "Unable to create the libmodbus context";
    }

  }

  // ---------------------------------------------------------------------------
  Device::Private::~Private() = default;

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
  Master::Master (const DataLinkLayer & sublayer) :
    Device (*new Private (this, sublayer)) {

  }

  // ---------------------------------------------------------------------------
  Master::~Master() {

  }

  // ---------------------------------------------------------------------------
  bool Master::open (int slave) {

    if (!isOpen() && !isNull()) {
      PIMP_D (Master);

      if (modbus_set_slave (d->ctx, slave) == 0) {

        d->slave = slave;
        return Device::open();
      }
      else {

        d->error = "Invalid slave ID";
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  int Master::slave() const {
    PIMP_D (const Master);

    return d->slave;
  }

  // ---------------------------------------------------------------------------
  int Master::readDiscreteInputs (int addr, bool * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_read_input_bits (d->ctx, d->address (addr), nb, (uint8_t *) dest);

      if (ret < 0) {
        d->error = modbus_strerror (errno);
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readCoils (int addr, bool * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_read_bits (d->ctx, d->address (addr), nb, (uint8_t *) dest);

      size_t tb = sizeof (bool);

      if (ret < 0) {
        d->error = modbus_strerror (errno);
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readInputRegistrers (int addr, uint16_t * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_read_input_registers (d->ctx, d->address (addr), nb, dest);

      if (ret < 0) {
        d->error = modbus_strerror (errno);
      }
      return ret;
    }

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::readRegistrers (int addr, uint16_t * dest, int nb) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_read_registers (d->ctx, d->address (addr), nb, dest);

      if (ret < 0) {

        d->error = modbus_strerror (errno);
      }
      return ret;
    }

    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeCoil (int addr, bool value) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_write_bit (d->ctx, d->address (addr), (uint8_t) value);

      if (ret < 0) {
        d->error = modbus_strerror (errno);
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeCoils (int addr, const bool * src, int nb) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_write_bits (d->ctx, d->address (addr), nb, (const uint8_t *) src);

      if (ret < 0) {
        d->error = modbus_strerror (errno);
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeRegistrer (int addr, uint16_t value) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_write_register (d->ctx, d->address (addr), value);

      if (ret < 0) {

        d->error = modbus_strerror (errno);
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeRegistrers (int addr, const uint16_t * src, int nb) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_write_registers (d->ctx, d->address (addr), nb, src);

      if (ret < 0) {

        d->error = modbus_strerror (errno);
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int Master::writeReadRegistrers (int waddr, const uint16_t * src, int wnb,
                                   int raddr, uint16_t * dest, int rnb) {

    if (isOpen()) {
      PIMP_D (Master);
      int ret = modbus_write_and_read_registers (d->ctx, d->address (waddr), wnb, src,
                d->address (raddr), rnb, dest);

      if (ret < 0) {

        d->error = modbus_strerror (errno);
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  //
  //                         Master::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Master::Private::Private (Master * q, const DataLinkLayer & sublayer) :
    Device::Private (q, sublayer), slave (0) {

    if (sublayer.type() == DataLinkLayer::Tcp) {
      slave = Master::TcpSlave;
    }
  }

  // ---------------------------------------------------------------------------
  Master::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  //
  //                         RtuLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  const char * RtuLayer::device() const {

    return host.c_str();
  }

  // ---------------------------------------------------------------------------
  int RtuLayer::baud() const {
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
  char RtuLayer::parity() const {
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
  int RtuLayer::stop() const {

    if (parity() == 'N') {

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
  const char * TcpLayer::node() const {

    return host.c_str();
  }

  // ---------------------------------------------------------------------------
  const char * TcpLayer::service() const {

    return settings.c_str();
  }

}

/* ========================================================================== */
