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
#include "rtulayer_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         RtuLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  RtuLayer::RtuLayer (RtuLayer::Private &dd) : NetLayer (dd) {}

  // ---------------------------------------------------------------------------
  RtuLayer::RtuLayer (const std::string & port, const std::string & settings) :
    NetLayer (*new Private (port, settings)) {}

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

    return connection();
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
    char p = 'E';
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
  //                         RtuLayer::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  RtuLayer::Private::Private (const std::string & port, const std::string & settings) :
    NetLayer::Private (Rtu, port, settings, MODBUS_RTU_MAX_ADU_LENGTH) {

    // RTU MUST BE 8-bits
    ctx = modbus_new_rtu (port.c_str(), RtuLayer::baud (settings),
                          RtuLayer::parity (settings), 8,
                          RtuLayer::stop (settings));
    if (! ctx) {

      throw std::invalid_argument (
        "Error: Unable to create RTU Modbus Backend("
        + port + "," + settings + ")\n" + lastError());
    }
  }

}

/* ========================================================================== */
