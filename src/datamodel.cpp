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
#include "datamodel_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                             DataModel Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  DataModel::DataModel (DataModel::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  DataModel::DataModel (int id) :
    d_ptr (new Private (this, id)) {}

  // ---------------------------------------------------------------------------
  DataModel::~DataModel() = default;

  // ---------------------------------------------------------------------------
  void DataModel::setPduAddressing (bool pdu) {
    PIMP_D (DataModel);

    d->pduAddressing = pdu;
  }

  // ---------------------------------------------------------------------------
  bool  DataModel::pduAddressing() const {
    PIMP_D (const DataModel);

    return d->pduAddressing;
  }

  // ---------------------------------------------------------------------------
  int DataModel::pduAddress (int dataAddr) const {
    PIMP_D (const DataModel);

    return dataAddr - (d->pduAddressing ? 0 : 1);
  }

  // ---------------------------------------------------------------------------
  int DataModel::slave() const {
    PIMP_D (const DataModel);

    return d->slave;
  }

  // ---------------------------------------------------------------------------
  // static
  void DataModel::setBoolArray (bool * dest, const uint8_t * src, size_t n) {
    
    modbus_set_bits_from_bytes ((uint8_t *) dest, 0, n, src);
  }

  // ---------------------------------------------------------------------------
  //
  //                         DataModel::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  DataModel::Private::Private (DataModel * q, int id) :
    q_ptr (q), pduAddressing (false), slave (id) {}

  // ---------------------------------------------------------------------------
  DataModel::Private::~Private() = default;

}

/* ========================================================================== */
