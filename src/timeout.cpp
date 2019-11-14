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
#include <modbuspp/timeout.h>
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         Timeout Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Timeout::Timeout (uint32_t s, uint32_t us) {

    setValue (s, us);
  }

  // ---------------------------------------------------------------------------
  Timeout::Timeout (const double & t) {

    setValue (t);
  }

  // ---------------------------------------------------------------------------
  void Timeout::setValue (const double & t) {

    m_sec = static_cast<uint32_t> (t);
    m_usec = (t - m_sec) * 1000000UL;
  }

  // ---------------------------------------------------------------------------
  void  Timeout::setValue (uint32_t s, uint32_t us) {

    if (us > 999999) {
      uint32_t ts = us / 1000000UL;
      s += ts;
      us -= ts * 1000000UL;
    }
    m_sec = s;
    m_usec = us;
  }

  // ---------------------------------------------------------------------------
  double Timeout::value() const {
    return static_cast<double> (m_sec) + static_cast<double> (m_usec) / 1000000UL;
  }
}

/* ========================================================================== */
