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
#ifdef _WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif

#include <modbuspp/message.h>
#include "tcplayer_p.h"
#include "config.h"

#if !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL 0
#endif

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         TcpLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  TcpLayer::TcpLayer (TcpLayer::Private &dd) : NetLayer (dd) {}

  // ---------------------------------------------------------------------------
  TcpLayer::TcpLayer (const std::string & host, const std::string & service) :
    NetLayer (*new Private (host, service)) {}

  // ---------------------------------------------------------------------------
  const std::string & TcpLayer::node() const {

    return connection();
  }

  // ---------------------------------------------------------------------------
  const std::string & TcpLayer::service() const {

    return settings();
  }

  // ---------------------------------------------------------------------------
  int TcpLayer::sendRawMessage (const Message * msg) {
    PIMP_D (TcpLayer);

    return send (modbus_get_socket (d->ctx), msg->adu(), msg->aduSize(),
                 MSG_NOSIGNAL);
  }

  // ---------------------------------------------------------------------------
  bool TcpLayer::prepareToSend (Message & msg) {

    if (msg.net() == Tcp && msg.size() >= 1) {
      PIMP_D (TcpLayer);

      uint8_t * adu = msg.adu();
      uint16_t s = msg.size() + 1;

      if (!msg.isResponse()) {
        adu[0] = d->transactionId >> 8;
        adu[1] = d->transactionId++ & 0xFF;
        adu[2] = 0;
        adu[3] = 0;
      }
      adu[4] = s >> 8;
      adu[5] = s & 0xFF;
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  //
  //                         TcpLayer::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  TcpLayer::Private::Private (const std::string & host, const std::string & service) :
    NetLayer::Private (Tcp, host, service, MODBUS_TCP_MAX_ADU_LENGTH),
    transactionId (1) {
    const char * node = NULL;

    if (host != "*") {
      node = host.c_str();
    }

    ctx = modbus_new_tcp_pi (node, service.c_str());
    if (! ctx) {

      throw std::invalid_argument (
        "Error: Unable to create TCP Modbus Backend(" +
        host + ":" + service + ")\n" + lastError());
    }
  }

}

/* ========================================================================== */
