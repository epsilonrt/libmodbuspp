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

#ifndef MODBUSPP_PRIVATE_H
#define MODBUSPP_PRIVATE_H

#include <cerrno>
#include <modbuspp.h>
#include <modbus.h>

namespace Modbus {

  class Device::Private {

    public:
      Private (Device * q, const DataLinkLayer & sublayer);
      virtual ~Private();
      int address (int addr);

      Device * const q_ptr;
      DataLinkLayer dll;
      modbus_t * ctx;
      bool isOpen;
      std::string error;
      bool pduAddressing;

      PIMP_DECLARE_PUBLIC (Device)
  };

  class Master::Private  : public Device::Private {

    public:
      Private (Master * q, const DataLinkLayer & sublayer);
      virtual ~Private();

      int slave;

      PIMP_DECLARE_PUBLIC (Master)
  };
}

/* ========================================================================== */
#endif /* MODBUSPP_PRIVATE_H defined */
