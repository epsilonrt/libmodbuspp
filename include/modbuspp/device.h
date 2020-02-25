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

#include <stdexcept>
#include <modbuspp/global.h>
#include <modbuspp/timeout.h>

namespace Modbus {

  class NetLayer;
  class RtuLayer;
  class TcpLayer;
  class Message;

  /**
   * @class Device
   * @brief Device connected to Modbus
   *
   * This class is the base class for Master and Slave.
   * It groups together their common properties and methods.
   *
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Device  {
    public:
      /**
       * @brief Constructor
       *
       * Constructs a Modbus device for the @b net network.
       *
       * For the Tcp backend :
       * - @b connection specifies the host name or IP
       * address of the host to connect to, eg. "192.168.0.5" , "::1" or
       * "server.com". A NULL value can be used to listen any addresses in server mode,
       * - @b settings is the service name/port number to connect to.
       * To use the default Modbus port use the string "502". On many Unix
       * systems, it’s convenient to use a port number greater than or equal
       * to 1024 because it’s not necessary to have administrator privileges.
       * .
       *
       * For the Rtu backend :
       * - @b connection specifies the name of the serial port handled by the
       *  OS, eg. "/dev/ttyS0" or "/dev/ttyUSB0",
       * - @b settings specifies communication settings as a string in the
       *  format BBBBPS. BBBB specifies the baud rate of the communication, PS
       *  specifies the parity and the bits of stop.
       *
       *  According to Modbus RTU specifications :
       *    - the possible combinations for PS are E1, O1 and N2.
       *    - the number of bits of data must be 8, also there is no possibility
       *      to change this setting
       *    .
       * .
       *
       * An exception std::invalid_argument is thrown if one of the parameters
       * is incorrect.
       */
      Device (Net net, const std::string & connection, const std::string & settings);

      /**
       * @brief constructor from a JSON file
       *
       * The file describes the configuration to apply, its format is as follows:
       *
       * @code
          {
            "modbuspp-device": {
              "mode": "tcp",
              "connection": "localhost",
              "settings": "1502",
              "recovery-link": true,
              "debug": true,
              "response-timeout": 500,
              "byte-timeout": 500
            }
          }
       * @endcode
       *
       * Only the first 3 elements are mandatory : @b mode, @b connection and @b settings,
       * the others are optional. In this example "modbuspp-device" is the key
       * to the JSON object that should be used. If the key provided is empty,
       * the file contains only one object corresponding to the configuration.
       *
       * @param jsonfile JSON file path
       * @param key name of the object key in the JSON file corresponding to
       * the configuration to be applied
       */
      explicit Device (const std::string & jsonfile, const std::string & key = std::string());

      /**
       * @brief Default constructor
       *
       * object cannot be used without calling @b setBackend() or @b setConfig()
       */
      Device ();

      /**
       * @brief Destructor
       *
       * The destructor closes the connection if it is open and releases all
       * affected resources.
       */
      virtual ~Device();

      /**
       * @brief Sets the backend for the @b net network.
       *
       * This function does nothing if isValid() is true and returns false.
       *
       * For the Tcp backend :
       * - @b connection specifies the host name or IP
       * address of the host to connect to, eg. "192.168.0.5" , "::1" or
       * "server.com". A NULL value can be used to listen any addresses in server mode,
       * - @b settings is the service name/port number to connect to.
       * To use the default Modbus port use the string "502". On many Unix
       * systems, it’s convenient to use a port number greater than or equal
       * to 1024 because it’s not necessary to have administrator privileges.
       * .
       *
       * For the Rtu backend :
       * - @b connection specifies the name of the serial port handled by the
       *  OS, eg. "/dev/ttyS0" or "/dev/ttyUSB0",
       * - @b settings specifies communication settings as a string in the
       *  format BBBBPS. BBBB specifies the baud rate of the communication, PS
       *  specifies the parity and the bits of stop.
       *
       *  According to Modbus RTU specifications :
       *    - the possible combinations for PS are E1, O1 and N2.
       *    - the number of bits of data must be 8, also there is no possibility
       *      to change this setting
       *    .
       * .
       *
       * An exception std::invalid_argument is thrown if one of the parameters
       * is incorrect.
       * @return true if successful.
       */
      bool setBackend (Net net, const std::string & connection, const std::string & setting);

      /**
       * @brief Set configuration from a JSON file
       *
       * This function does nothing if isValid() is true and returns false.
       * 
       * The file describes the configuration to apply, its format is as follows:
       *
       * @code
          {
            "modbuspp-device": {
              "mode": "tcp",
              "connection": "localhost",
              "settings": "1502",
              "recovery-link": true,
              "debug": true,
              "response-timeout": 500,
              "byte-timeout": 500
            }
          }
       * @endcode
       *
       * Only the first 3 elements are mandatory : @b mode, @b connection and @b settings,
       * the others are optional. In this example "modbuspp-device" is the key
       * to the JSON object that should be used. If the key provided is empty,
       * the file contains only one object corresponding to the configuration.
       *
       * @param jsonfile JSON file path
       * @param key name of the object key in the JSON file corresponding to
       * the configuration to be applied
       * @return true if successful.
       */
      bool setConfig (const std::string & jsonfile, const std::string & key = std::string());

      /**
       * @brief Returns the connection used
       * 
       * Serial port or host depending on the backend.
       */
      std::string connection() const;
      
      /**
       * @brief Returns the connection settings
       * 
       * IP port or speed, parity and stop bit depending on the backend
       */
      std::string settings() const;

      /**
       * @brief Establish a Modbus connection
       *
       * This function shall establish a connection to a Modbus server,
       * a network or a bus
       *
       * @return true if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual bool open();

      /**
       * @brief Close a Modbus connection
       */
      virtual void close();

      /**
       * @brief Returns true if the device is open; otherwise returns false.
       * @sa open()
       */
      virtual bool isOpen() const;

      /**
       * @brief Returns true if the device is connected; otherwise returns false.
       *
       * in RTU mode, this function returns the same value as isOpen().
       *
       * in TCP mode, this function returns true if a peer-to-peer TCP
       * connection is currently established. Indeed, in server mode (slave),
       * calling the open() function puts the Device in passive waiting mode,
       * so that it is open but not connected. It is when a client connects
       * that the function returns true.
       * @sa isOpen()
       */
      bool isConnected() const;

      /**
       * @brief returns true if backend is set
       */
      bool isValid() const;

      /**
       * @brief Set the link recovery  mode after disconnection.
       *
       * When is set, the library will attempt an immediate reconnection when
       * the connection is reset by peer.
       * @return true if successful.
       */
      virtual bool setRecoveryLink (bool recovery = true);

      /**
       * @brief Returns true if link recovery  mode is set; otherwise returns false.
       */
      bool recoveryLink() const;

      /**
       * @brief Flush non-transmitted data
       *
       * This function shall discard data received but not read to the socket or
       * file descriptor associated to this connection.
       *
       * @return 0 or the number of flushed bytes if successful.
       * Otherwise it shall return -1 and set errno.
       */
      int flush();

      /**
       * @brief Set timeout for response
       *
       * This function shall set the timeout interval used to wait for
       * a response. When a byte timeout is set, if elapsed time for the first
       * byte of response is longer than the given timeout, an `ETIMEDOUT` error
       * will be raised by the function waiting for a response. When byte
       * timeout is disabled, the full confirmation response must be received
       * before expiration of the response timeout.
       *
       * @param timeout reference on the variable that will contain the new
       * timeout in seconds.
       * @return true if successful.
       * @sa responseTimeout()
       */
      bool setResponseTimeout (const double & timeout);

      /**
       * @brief Get timeout for response
       *
       * @return timeout for response in seconds.
       * @sa setResponseTimeout()
       */
      double responseTimeout();

      /**
       * @brief Set timeout for response
       *
       * This function shall set the timeout interval used to wait for
       * a response. When a byte timeout is set, if elapsed time for the first
       * byte of response is longer than the given timeout, an `ETIMEDOUT` error
       * will be raised by the function waiting for a response. When byte
       * timeout is disabled, the full confirmation response must be received
       * before expiration of the response timeout.
       *
       * @param timeout reference on the variable that will contain the new timeout value
       * @return true if successful.
       * @sa responseTimeout()
       */
      bool setResponseTimeout (const Timeout & timeout);

      /**
       * @brief Get timeout for response
       *
       * This function shall return the timeout interval used to wait
       * for a response in the @b timeout argument.
       * @param timeout reference on the variable that will contain the timeout value
       * @return true if successful.
       * @sa setResponseTimeout()
       */
      bool responseTimeout (Timeout & timeout);

      /**
       * @brief Set timeout between bytes
       *
       * This function shall set the timeout interval between two consecutive
       * bytes of the same message. The timeout is an upper bound on the amount
       * of time elapsed before *select()* returns, if the time elapsed is longer
       * than the defined timeout, an `ETIMEDOUT` error will be raised by the
       * function waiting for a response.
       *
       * @param timeout reference on the variable that will contain the new
       * timeout in seconds.
       * @return true if successful.
       * @sa byteTimeout()
       */
      bool setByteTimeout (const double & timeout);

      /**
       * @brief Get timeout between bytes
       *
       * @return timeout between bytes in seconds.
       * @sa setByteTimeout()
       */
      double byteTimeout();

      /**
       * @brief Set timeout between bytes
       *
       * This function shall set the timeout interval between two consecutive
       * bytes of the same message. The timeout is an upper bound on the amount
       * of time elapsed before *select()* returns, if the time elapsed is longer
       * than the defined timeout, an `ETIMEDOUT` error will be raised by the
       * function waiting for a response.
       *
       * @param timeout reference on the variable that will contain the new timeout value
       * @return true if successful.
       * @sa byteTimeout()
       */
      bool setByteTimeout (const Timeout & timeout);

      /**
       * @brief Get timeout between bytes
       *
       * This function shall store the timeout interval between two consecutive
       * bytes of the same message in the @b timeout argument.
       *
       * @param timeout reference on the variable that will contain the timeout value
       * @return true if successful.
       * @sa setByteTimeout()
       */
      bool byteTimeout (Timeout & timeout);

      /**
       * @brief Underlying layer used (backend)
       *
       * This function allows to know the underlying layer used.
       */
      Net net() const;

      /**
       * @brief underlying RTU layer (backend)
       *
       * This function shall return the RTU layer if it is the layer used by
       * the device. If it does not, a @b std::domain_error exception is thrown.
       */
      RtuLayer & rtu();

      /**
       * @brief underlying TCP layer (backend)
       *
       * This function shall return the TCP layer if it is the layer used by
       * the device. If it does not, a @b std::domain_error exception is thrown.
       */
      TcpLayer & tcp();

      /**
       * @brief underlying backend
       */
      NetLayer & backend() const;

      /**
       * @brief Send a request/response @b msg  via the socket of the @b context()
       * 
       * This function can be used to send message not handled by the library.
       * The message is transmitted "raw", without any modification if 
       * @b prepareBefore is false, otherwise the message is modified to add a 
       * header (for TCP-IP) or a CRC (for RTU).
       * 
       * @return The function shall return the full message length, if successful. 
       * Otherwise it shall return -1 and set errno
       * 
       * @warning This function is not supported in RTU mode on Windows™.
       */
      int sendRawMessage (Message * msg, bool prepareBefore = false);
      
      /**
       * @overload
       */
      int sendRawMessage (Message & msg, bool prepareBefore = false);

      /**
       * @brief Set debug flag
       *
       * This function function shall set the debug flag by using the argument
       * @b flag. By default, the boolean flag is set to false.
       * When the @b flag value is set to true, many verbose messages are
       * displayed on stdout and stderr.
       * For example, this flag is useful to display the bytes of the
       * Modbus messages :
       * @code
       * [00][14][00][00][00][06][12][03][00][6B][00][03]
       * Waiting for a confirmation...
       * <00><14><00><00><00><09><12><03><06><02><2B><00><00><00><00>
       * @endcode
       *
       * @return true if successful.
       * Otherwise it shall return false and set errno.
       */
      bool setDebug (bool debug = true);

      /**
       * @brief Return the debug flag
       */
      bool debug () const;

      /**
       * @brief last error message
       *
       * This function shall return the error message corresponding to
       * the last error. This function must be called right after the
       * instruction that triggered an error.
       */
      static std::string lastError();

    protected:
      class Private;
      Device (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Device)
  };
}

/* ========================================================================== */
