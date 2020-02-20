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
#pragma once

#include <modbuspp/global.h>

namespace Modbus {

  /**
   * @class Timeout
   * @brief Represents a timeout
   * 
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Timeout {
    public:

      /**
       * @brief Default constructor
       * @param s seconds
       * @param us microseconds
       */
      Timeout (uint32_t s = 1, uint32_t us = 0);

      /**
       * @brief Constructor from a double
       * @param t time in seconds
       */
      Timeout (const double & t);

      /**
       * @brief Return the timeout in seconds
       */
      double value() const;

      /**
       * @brief Set value from a double
       * @param t time in seconds
       */
      void setValue (const double & t);

      /**
       * @brief Set the value from the integer part and the decimal part
       * @param s seconds
       * @param us microseconds, if this value is greater than 999999, it is
       * adjusted as well as @b s.
       */
      void setValue (uint32_t s, uint32_t us);

      /**
       * @brief Overload of the assignment operator from a double value
       * @param t time in seconds
       */
      Timeout& operator= (const double& t) {
        setValue (t);
        return *this;
      }

      /**
       * @brief Return the number of seconds
       */
      uint32_t sec() const {
        return m_sec;
      }

      /**
       * @brief Return the number of microseconds
       *
       * in the range 0 to 999999.
       */
      uint32_t usec() const {
        return m_usec;
      }

      friend class Device;

    private:
      uint32_t m_sec;
      uint32_t m_usec;
  };
}

/* ========================================================================== */
