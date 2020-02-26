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

#include <modbuspp/netlayer.h>

namespace Modbus {

  /**
   * @class RtuLayer
   * @brief RTU serial link layer
   *
   * This class can not and should not be instantiated by the user.
   * It provides access to properties and methods specific to the RTU layer.
   *
   * An instance of this class is created by the constructor @b Device::Device()
   * of the @b Device class (or its derived classes) if the RTU layer is selected.
   *
   * Access to this instance is done using the Device::rtu() method.
   *
   * @sa Device::Device()
   * @sa Device::rtu()
   * 
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class RtuLayer : public NetLayer {
    public:

      /**
       * @brief Constructor
       */
      RtuLayer (const std::string & port, const std::string & settings);

      /**
       * @brief Name of the serial port
       *
       * This property specifies the name of the serial port handled by the
       *  OS, eg. "/dev/ttyS0" or "/dev/ttyUSB0".
       */
      const std::string & port() const;

      /**
       * @brief Return the baudrate
       */
      int baud() const;

      /**
       * @brief Return the parity
       */
      char parity() const;

      /**
       * @brief Return the bits of stop
       */
      int stop() const;

      /**
       * @brief Get the current serial mode
       *
       * This function shall return the serial mode currently used.
       *
       * - @b Rs232: the serial line is set for RS232 communication. RS-232
       *  (Recommended Standard 232) is the traditional name for a series of
       *  standards  for serial binary single-ended data and control signals
       *  connecting between a DTE (Data Terminal Equipment) and a DCE
       *  (Data Circuit-terminating  Equipment).
       *  It is commonly used in computer serial ports
       * - @b Rs485: the serial line is set for RS485 communication. EIA-485,
       *  also known as TIA/EIA-485 or RS-485, is a standard defining the electrical
       *  characteristics of drivers and receivers for use in balanced digital
       *  multipoint systems. This standard is widely used for communications in
       *  industrial  automation because it can be used effectively over long
       *  distances and in  electrically noisy environments.
       * .
       * This function is only available on Linux kernels 2.6.28 onwards.
       *
       * @return return the current mode if successful.
       * Otherwise it shall return @b UnknownMode (-1) and set errno.
       * @sa setSerialMode()
       */
      SerialMode serialMode();

      /**
       * @brief Set the serial mode
       *
       * This function shall set the selected serial mode @b mode.
       *
       * @return true if successful.
       * Otherwise it shall return false and set errno.
       * @sa serialMode()
       */
      bool setSerialMode (SerialMode mode);

      /**
       * @brief Get the current RTS mode
       *
       * This function shall get the current Request To Send mode
       *
       * @return the current RTS mode if successful.
       * Otherwise it shall return @b UnknownRts (-1) and set errno.
       * @sa setRts()
       */
      SerialRts rts();

      /**
       * @brief Set the RTS mode
       *
       * This function shall set the Request To Send mode to communicate on a
       * RS485 serial bus.
       *
       * @return true if successful.
       * Otherwise it shall return false and set errno.
       * @sa rts()
       */
      bool setRts (SerialRts rts);

      /**
       * @brief Get the current RTS delay
       *
       * This function shall get the current Request To Send delay period.
       * @return the current RTS delay in microseconds if successful.
       * Otherwise it shall return -1 and set errno.
       * @sa setRtsDelay()
       */
      int rtsDelay();

      /**
       * @brief Set the RTS delay
       *
       * This function shall set the Request To Send delay period in microseconds.
       *
       * @return true if successful.
       * Otherwise it shall return false and set errno.
       * @sa rtsDelay()
       */
      bool setRtsDelay (int us);

      /**
       * @overload
       *
       * @warning This function is not supported by Windows !
       */
      virtual int sendRawMessage (const Message * msg);
      
      /**
       * @overload
       */
      virtual bool prepareToSend (Message & msg);

      /**
       * @brief Extracts the baudrate from a settings string.
       * @return the baudrate found. if no value is found, returns the default
       * value, ie 19200.
       */
      static int baud (const std::string & settings);

      /**
       * @brief Extracts the parity from a settings string.
       * @return the parity found. if no value is found, returns the default
       * value, ie E for Even parity.
       */
      static char parity (const std::string & settings);

      /**
       * @brief Return the stop bits from a settings string.
       *
       * @return the number returned is determined based on the parity found.
       * If the parity is None, this function returns 2, otherwise returns 1.
       */
      static int stop (const std::string & settings);
      
      /**
       * @brief Performing Modbus CRC16 generation of the buffer @b buf
       */
      static uint16_t crc16 (uint8_t * buf, uint16_t count);

    protected:
      class Private;
      RtuLayer (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (RtuLayer)
  };
}

/* ========================================================================== */
