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
#include <modbuspp/message.h>
#include <modbus-ascii.h>
#include "asciilayer_p.h"
#include "config.h"
#include "modbuspp/global.h"

#ifndef _WIN32
#include <unistd.h>
#endif

namespace Modbus {
  // ---------------------------------------------------------------------------
  //
  //                         AsciiLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  AsciiLayer::AsciiLayer (AsciiLayer::Private &dd) : NetLayer (dd) {}

  // ---------------------------------------------------------------------------
  AsciiLayer::AsciiLayer (const std::string & port, const std::string & settings) :
    NetLayer (*new Private (port, settings)) {}

  // ---------------------------------------------------------------------------
  SerialMode AsciiLayer::serialMode() {
    PIMP_D (AsciiLayer);

    int m = modbus_serial_get_serial_mode (d->ctx);
    if (m != -1) {
      return static_cast<SerialMode> (m);
    }
    return UnknownMode;
  }

  // ---------------------------------------------------------------------------
  bool AsciiLayer::setSerialMode (SerialMode mode) {
    PIMP_D (AsciiLayer);

    return (modbus_serial_set_serial_mode (d->ctx, static_cast<int> (mode)) != -1);
  }

  // ---------------------------------------------------------------------------
  SerialRts AsciiLayer::rts() {
    PIMP_D (AsciiLayer);

    int r = modbus_serial_get_rts (d->ctx);
    if (r != -1) {

      return static_cast<SerialRts> (r);
    }
    return UnknownRts;
  }

  // ---------------------------------------------------------------------------
  bool AsciiLayer::setRts (SerialRts r) {
    PIMP_D (AsciiLayer);

    return (modbus_serial_set_rts (d->ctx, static_cast<int> (r)) != -1);
  }

  // ---------------------------------------------------------------------------
  int AsciiLayer::rtsDelay() {
    PIMP_D (AsciiLayer);

    return modbus_serial_get_rts_delay (d->ctx);
  }

  // ---------------------------------------------------------------------------
  bool AsciiLayer::setRtsDelay (int delay) {
    PIMP_D (AsciiLayer);

    return (modbus_serial_set_rts_delay (d->ctx, delay) != -1);
  }

  // ---------------------------------------------------------------------------
  const std::string & AsciiLayer::port() const {

    return connection();
  }

  // ---------------------------------------------------------------------------
  int AsciiLayer::baud() const {
    PIMP_D (const AsciiLayer);

    return baud (d->settings);
  }

  // ---------------------------------------------------------------------------
  char AsciiLayer::parity() const {
    PIMP_D (const AsciiLayer);

    return parity (d->settings);
  }

  // ---------------------------------------------------------------------------
  int AsciiLayer::stop() const {
    PIMP_D (const AsciiLayer);

    return stop (d->settings);
  }

  // ---------------------------------------------------------------------------
  int AsciiLayer::sendRawMessage (const Message * msg) {
    PIMP_D (const AsciiLayer);
#if defined(_WIN32)
    errno = ENOTSUP;
    return -1;
#else
#if MODBUSPP_HAVE_TIOCM_RTS
    if (rts() != RtsNone) {
      ssize_t size;
      SerialRts r = (rts() == RtsDown) ? RtsUp : RtsDown; // complement the Rts state

      setRts (r);
      usleep (rtsDelay());

      size = write (modbus_get_socket (d->ctx), msg->adu(), msg->aduSize());

      usleep (d->oneByteTime * msg->aduSize() + rtsDelay());
      setRts ( (r == RtsDown) ? RtsUp : RtsDown); // restore initial state

      return size;
    }
    else {
#endif
      return write (modbus_get_socket (d->ctx), msg->adu(), msg->aduSize());
#if MODBUSPP_HAVE_TIOCM_RTS
    }
#endif
#endif
  }

static char nibble_to_hex_ascii(uint8_t nibble)
{
    char c;

    if (nibble < 10) {
        c = nibble + '0';
    } else {
        c = nibble - 10 + 'A';
    }
    return c;
}

  // ---------------------------------------------------------------------------
  bool AsciiLayer::prepareToSend (Message & msg) {
	
    if (msg.net() == Ascii && msg.size() >= 1) {
      size_t aduSize = msg.aduSize();
      uint8_t * adu = msg.adu();

      /* Skip colon */
      uint8_t lrc = lrc8(adu + 1, aduSize- 1);
      adu[aduSize++] = lrc;

    uint8_t ascii_adu[MODBUS_ASCII_MAX_ADU_LENGTH];
    memset(ascii_adu, 0, MODBUS_ASCII_MAX_ADU_LENGTH);
    ssize_t i, j = 0;

    for (i = 0; i < aduSize; i++) {
        if ((i == 0 && adu[i] == ':') ||
            (i == aduSize - 2 && adu[i] == '\r') ||
            (i == aduSize - 1 && adu[i] == '\n')) {
            ascii_adu[j++] = adu[i];
        } else {
            ascii_adu[j++] = nibble_to_hex_ascii(adu[i] >> 4);
            ascii_adu[j++] = nibble_to_hex_ascii(adu[i] & 0x0f);
        }
    }
    ascii_adu[j++] = '\r';
    ascii_adu[j++] = '\n';
    ascii_adu[j] = '\0';

      msg.setAduSize (j);

      std::memcpy(adu, ascii_adu, j);

      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool AsciiLayer::checkMessage (const Message & msg) {

    return lrc8 (msg.adu(), msg.aduSize() - 2) == msg.lrc ();
  }

  // ---------------------------------------------------------------------------
  // static
  int AsciiLayer::baud (const std::string & settings) {
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
  char AsciiLayer::parity (const std::string & settings) {
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
  int AsciiLayer::stop (const std::string & settings) {

    if (parity (settings) == 'N') {

      return 2;
    }
    return 1;
  }

  // ---------------------------------------------------------------------------
  // static
   uint8_t AsciiLayer::lrc8(const uint8_t *buffer, uint16_t buffer_length)
   {
       uint8_t lrc = 0;
       while (buffer_length--) {
           lrc += *buffer++;
       }
       /* Return two's complementing of the result */
       return -lrc;
   }

  // ---------------------------------------------------------------------------
  //
  //                         AsciiLayer::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  AsciiLayer::Private::Private (const std::string & port, const std::string & settings) :
    NetLayer::Private (Ascii, port, settings, MODBUS_ASCII_MAX_ADU_LENGTH) {

    // RTU MUST BE 8-bits
    ctx = modbus_new_ascii (port.c_str(), AsciiLayer::baud (settings),
                          AsciiLayer::parity (settings), 8,
                          AsciiLayer::stop (settings));

    if (! ctx) {

      throw std::invalid_argument (
        "Unable to create ASCII Modbus Backend("
        + port + "," + settings + ")\n" + lastError());
    }
    oneByteTime = modbus_serial_get_rts_delay (ctx);
  }
}

/* ========================================================================== */
