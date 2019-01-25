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

#ifndef MODBUSPP_H
#define MODBUSPP_H

#include <string>
#include <modbus.h>
#include <modbuspp-data.h>
#include "global.h"

/**
 *
 */
namespace Modbus {

  const int Broadcast = MODBUS_BROADCAST_ADDRESS; ///< Modbus Broadcast Address
  const int TcpSlave = MODBUS_TCP_SLAVE; ///< Can be used in TCP mode to restore the default value
  const int Unknown = -1; ///< Value corresponding to an unknown parameter

  /**
   * @enum Net
   * @brief Enumeration of network layers (backends) used by Modbus
   */
  enum Net {
    
    /**
     * @brief RTU backend
     * 
     * The RTU backend (Remote Terminal Unit) is used in serial communication 
     * and makes use of a compact, binary representation of the data for 
     * protocol communication. 
     */
    Rtu,
    /**
     * @brief TCP backend
     * 
     * The TCP PI (Protocol Independent) backend implements a Modbus variant 
     * used for communications over TCP IPv4 and IPv6 networks. 
     */
    Tcp,
    NoNet = Unknown  ///< Unknown network backend
  };

  /**
   * @enum SerialMode
   * @brief Enumeration of RTU serial link modes
   */
  enum SerialMode {
    /**
     * @brief RS232
     * 
     * RS-232 (Recommended Standard 232) is the traditional name for a series 
     * of standards for serial binary single-ended data and control signals 
     * connecting between a DTE (Data Terminal Equipment) and a DCE 
     * (Data Circuit-terminating Equipment). 
     * It is commonly used in computer serial ports
     **/
    Rs232 = MODBUS_RTU_RS232,
    /**
     * @brief RS485
     * 
     * EIA-485, also known as TIA/EIA-485 or RS-485, is a standard defining 
     * the electrical characteristics of drivers and receivers for use in 
     * balanced digital multipoint systems. This standard is widely used for 
     * communications in industrial automation because it can be used 
     * effectively over long distances and in electrically noisy environments.
     */ 
    Rs485 = MODBUS_RTU_RS485,
    UnknownMode = Unknown  ///< Unknown serial mode
  };

  /**
   * @enum SerialRts
   * @brief  RTS mode in RTU
   * 
   * Request To Send mode to communicate on a RS485 serial bus. \n
   * 
   * By default, the mode is set to \c RtsNone and no signal is issued before 
   * writing data on the wire. \n
   * 
   * To enable the RTS mode, the values \c RtsUp or \c RtsDown must be used, 
   * these modes enable the RTS mode and set the polarity at the same time. 
   * 
   * When \c RtsUp is used, an ioctl call is made with RTS flag enabled then 
   * data is written on the bus after a delay of 1 ms, then another ioctl call 
   * is made with the RTS flag disabled and again a delay of 1 ms occurs. \n
   *
   * The \c RtsDown mode applies the same procedure but with an inverted RTS flag.
   */
  enum SerialRts {
    RtsNone = MODBUS_RTU_RTS_NONE,
    RtsUp = MODBUS_RTU_RTS_UP,
    RtsDown = MODBUS_RTU_RTS_DOWN,
    UnknownRts = Unknown   ///< Unknown RTS mode
  };

  /**
   * @class Timeout
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   * @brief Represents a timeout
   */
  class Timeout {
    public:
      Timeout (uint32_t s = 0, uint32_t us = 0) :
        sec (s), usec (us) {}
      uint32_t sec;   ///< number of seconds
      uint32_t usec; ///< number of microseconds, must be in the range 0 to 999999.
  };


  class RtuLayer;
  class TcpLayer;

  /**
   * @class Device
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   * @brief Device connected to Modbus
   * 
   * This class is the base class for Master and Slave. 
   * It groups together their common properties and methods.
   */
  class Device  {
  public:

      /**
       * @brief Constructor
       * 
       * Constructs a Modbus device for the \c net network.
       * 
       * For the Tcp backend :
       * - \c connection specifies the host name or IP 
       * address of the host to connect to, eg. "192.168.0.5" , "::1" or 
       * "server.com". A NULL value can be used to listen any addresses in server mode,
       * - \c settings is the service name/port number to connect to. 
       * To use the default Modbus port use the string "502". On many Unix 
       * systems, it’s convenient to use a port number greater than or equal 
       * to 1024 because it’s not necessary to have administrator privileges.
       * .
       * 
       * For the Rtu backend :
       * - \c connection specifies the name of the serial port handled by the 
       *  OS, eg. "/dev/ttyS0" or "/dev/ttyUSB0",
       * - \c parameters specifies communication parameters as a string in the 
       *  format BBBBPS. BBBB specifies the baud rate of the communication, PS
       *  specifies the parity and specifies the bits of stop. \n
       *  According to Modbus RTU specifications :
       *    - the possible combinations for PS are E1, O1 and N2.
       *    - the number of bits of data must be 8, also there is no possibility 
       *      to change this setting
       *    .
       * .
       */
      Device (Net net = Tcp, const std::string & connection = "*",
              const std::string & settings = "502");
      
      /**
       * @brief Copy constructor
       */
      Device (const Device & other);
      
      /**
       * @brief Destructor
       * 
       * The destructor closes the connection if it is open and releases all 
       * affected resources.
       */
      virtual ~Device();

      /**
       * @brief Swaps the managed objects and associated deleters of *this and another
       */
      void swap (Device &other);
      
      /**
       * @brief Copy assignment operator
       */
      Device& operator= (const Device &other);

      /**
       * @brief 
       * @return 
       */
      virtual bool open();
      virtual void close();
      bool flush();
      bool isOpen() const;

      int slave() const;
      bool setSlave (int id);

      bool setResponseTimeout (const Timeout & timeout);
      bool responseTimeout (Timeout & timeout);
      bool setByteTimeout (const Timeout & timeout);
      bool byteTimeout (Timeout & timeout);

      bool setDebug (bool debug = true);

      void setPduAdressing (bool pduAdressing = true);
      bool pduAdressing() const;

      bool isNull() const;
      
      Net net() const;
      RtuLayer & rtu();
      TcpLayer & tcp();

      static std::string lastError();

      friend class RtuLayer;
      friend class TcpLayer;

    protected:
      class Private;
      Device (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Device)
  };

  /**
   * @class Master
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   * @brief Master connected to Modbus (Client)
   * 
   * The Modbus master is the only one able to initiate a transaction with 
   * the slaves. This class therefore makes it possible to read or write in 
   * Modbus slaves.
   */
  class Master : public Device {

    public:
      /**
       * @brief Constructor
       * 
       * Constructs a Modbus master for the \c net network.
       * 
       * For the Tcp backend :
       * - \c connection specifies the host name or IP 
       * address of the host to connect to, eg. "192.168.0.5" , "::1" or 
       * "server.com". A NULL value can be used to listen any addresses in server mode,
       * - \c settings is the service name/port number to connect to. 
       * To use the default Modbus port use the string "502". On many Unix 
       * systems, it’s convenient to use a port number greater than or equal 
       * to 1024 because it’s not necessary to have administrator privileges.
       * .
       * 
       * For the Rtu backend :
       * - \c connection specifies the name of the serial port handled by the 
       *  OS, eg. "/dev/ttyS0" or "/dev/ttyUSB0",
       * - \c parameters specifies communication parameters as a string in the 
       *  format BBBBPS. BBBB specifies the baud rate of the communication, PS
       *  specifies the parity and specifies the bits of stop. \n
       *  According to Modbus RTU specifications :
       *    - the possible combinations for PS are E1, O1 and N2.
       *    - the number of bits of data must be 8, also there is no possibility 
       *      to change this setting
       *    .
       * .
       */
      Master (Net net = Tcp, const std::string & connection = "*",
              const std::string & settings = "502");
      /**
       * @brief Destructor
       * 
       * The destructor closes the connection if it is open and releases all 
       * affected resources.
       */
      virtual ~Master();

      /**
       * @brief Read many discrete inputs (input bits)
       *
       * This function shall read the content of the \c nb input bits to the 
       * address \c addr of the remote device.  The result of reading is stored 
       * in \c dest array as  boolean. \n
       * The function uses the Modbus function code 0x02 (read input status).
       * 
       * @return the number of read input status if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int readDiscreteInputs (int addr, bool * dest, int nb = 1);

      /**
       * @brief Read many coils (bits)
       *
       * This function shall read the status of the \c nb bits (coils) to the 
       * address \c addr of the remote device. The result of reading is stored
       * in \c dest array as boolean. \n
       * The function uses the Modbus function code 0x01 (read coil status).
       * 
       * @return the number of read bits if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int readCoils (int addr, bool * dest, int nb = 1);

      /**
       * @brief Write a single coil (bit)
       *
       * This function shall write the status of src at the address addr of
       * the remote device. \n
       * The function uses the Modbus function code 0x05 (force single coil).
       * 
       * @return 1 if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int writeCoil (int addr, bool src);

      /**
       * @brief Write many coils (bits)
       *
       * This function shall write the status of the \c nb bits (coils) from 
       * \c src at the address \c addr of the remote device. 
       * The \c src array must contains booelans. \n
       * The function uses the Modbus function code 0x0F (force multiple coils).
       * 
       * @return the number of written bits if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int writeCoils (int addr, const bool * src, int nb);

      /**
       * @brief Read many input registers
       * 
       * This function shall read the content of the \c nb input registers to 
       * the address \c addr of the remote device. \n
       * The result of reading is stored in \c dest array as word values (16 bits). \n
       * The function uses the Modbus function code 0x04 (read input registers). 
       * The holding registers and input registers have different historical 
       * meaning, but nowadays it's more common to use holding registers only.
       * 
       * @return return the number of read input registers if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int readInputRegisters (int addr, uint16_t * dest, int nb = 1);

      /**
       * @brief Read many registers
       * 
       * This function shall read the content of the nb holding registers to 
       * the address \c addr of the remote device. \n
       * The result of reading is stored in \c dest array as word values (16 bits). \n
       * The function uses the Modbus function code 0x03 (read holding registers).
       * 
       * @return return the number of read registers if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int readRegisters (int addr, uint16_t * dest, int nb = 1);
 
      /**
       * @brief Write a single register
       * 
       * This function shall write the value of \c value holding registers at 
       * the address \c addr of the remote device. \n
       * The function uses the Modbus function code 0x06 (preset single register).
       *
       * @return 1 if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int writeRegister (int addr, uint16_t value);

      /**
       * @brief Write many registers
       *
       * This function shall write the content of the \c nb holding registers 
       * from the array \c src at address \c addr of the remote device. \n
       * The function uses the Modbus function code 0x10 (preset multiple registers).
       * 
       * @return number of written registers if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int writeRegisters (int addr, const uint16_t * src, int nb);

      /**
       * @brief Write and read many registers in a single transaction
       *
       * This function shall write the content of the \c write_nb holding 
       * registers from the array 'src' to the address \c write_addr of the 
       * remote device then shall read the content of the \c read_nb holding 
       * registers to the address \c read_addr of the remote device. 
       * The result of reading is stored in \c dest array as word values (16 bits).
       * 
       * You must take care to allocate enough memory to store the results in 
       * \c dest (at least \c nb * sizeof(uint16_t)).
       * 
       * The function uses the Modbus function code 0x17 (write/read registers).
       * 
       * @return the number of read registers if successful. 
       * Otherwise it shall return -1 and set errno.
       */
      int writeReadRegisters (int write_addr, const uint16_t * src, int write_nb,
                              int  read_addr, uint16_t * dest, int read_nb);

      /**
       * @brief
       * @param addr
       * @param dest
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      inline int readDiscreteInput (int addr, bool & dest) {

        return readDiscreteInputs (addr, &dest, 1);
      }

      /**
       * @brief 
       * @param addr
       * @param dest
       * @return 
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      inline int readCoil (int addr, bool & dest) {

        return readCoils (addr, &dest, 1);
      }

      /**
       * @brief
       * @param addr
       * @param dest
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      inline int readInputRegister (int addr, uint16_t & dest) {

        return readInputRegisters (addr, &dest, 1);
      }

      /**
       * @brief
       * @param addr
       * @param dest
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      inline int readRegister (int addr, uint16_t & dest) {

        return readRegisters (addr, &dest, 1);
      }

      /**
       * @brief
       * @param addr
       * @param dest
       * @param nb
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      template <typename T, Endian e> int readInputRegisters (int addr, Data<T, e> * dest, int nb = 1) {
        int ret = 0;

        for (int i = 0; i < nb; i++) {

          int r = readInputRegister (addr, dest[i]);
          if (r > 0) {

            addr += r;
            ret  += r;
          }
          else {

            return r;
          }
        }
        return ret;
      }

      /**
       * @brief
       * @param addr
       * @param dest
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      template <typename T, Endian e> int readInputRegister (int addr, Data<T, e> & dest) {
        int s = dest.registers().size();

        int r = readInputRegisters (addr, dest.registers().data(), s);
        if (r == s) {

          dest.updateValue();
        }
        return r;
      }

      /**
       * @brief
       * @param addr
       * @param dest
       * @param nb
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      template <typename T, Endian e> int readRegisters (int addr, Data<T, e> * dest, int nb = 1) {
        int ret = 0;

        for (int i = 0; i < nb; i++) {

          int r = readRegister (addr, dest[i]);
          if (r > 0) {

            addr += r;
            ret  += r;
          }
          else {

            return r;
          }
        }
        return ret;
      }

      /**
       * @brief
       * @param addr
       * @param dest
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      template <typename T, Endian e> int readRegister (int addr, Data<T, e> & dest) {
        int s = dest.registers().size();

        int r = readRegisters (addr, dest.registers().data(), s);
        if (r == s) {

          dest.updateValue();
        }
        return r;
      }

      /**
       * @brief
       * @param addr
       * @param src
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      template <typename T, Endian e> int writeRegister (int addr, const Data<T, e> & src) {

        return writeRegisters (addr, src.registers().data(), src.registers().size());
      }

      /**
       * @brief
       * @param addr
       * @param src
       * @param nb
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      template <typename T, Endian e> int writeRegisters (int addr, const Data<T, e> * src, int nb = 1) {
        int ret = 0;

        for (int i = 0; i < nb; i++) {

          int r = writeRegister (addr, src[i]);
          if (r > 0) {

            addr += r;
            ret  += r;
          }
          else {

            return r;
          }
        }
        return ret;
      }

      /**
       * @brief
       * @param waddr
       * @param src
       * @param wnb
       * @param raddr
       * @param dest
       * @param rnb
       * @return
       */
#ifdef __DOXYGEN__
#else  /* __DOXYGEN__ not defined */
#endif /* __DOXYGEN__ defined */
      template <typename T, Endian e> int writeReadRegisters (int waddr, const Data<T, e> * src, int wnb,
          int raddr, Data<T, e> * dest, int rnb) {
        return 0;
      }

    protected:
      class Private;
      Master (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Master)
  };


  /**
   * @class NetLayer
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   * @brief
   */
  class NetLayer  {

    protected:
      class Private;
      NetLayer (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (NetLayer)
  };

  /**
   * @class TcpLayer
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   * @brief
   */
  class TcpLayer : public NetLayer {
    public:
      const std::string & node() const;
      const std::string & service() const;

      friend class Device;

    protected:
      class Private;
      TcpLayer (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (TcpLayer)
  };

  /**
   * @class RtuLayer
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   * @brief
   */
  class RtuLayer : public NetLayer {
    public:
      const std::string & port() const;
      int baud() const;
      char parity() const;
      int stop() const;

      SerialMode serialMode();
      bool setSerialMode (SerialMode mode);
      SerialRts rts();
      bool setRts (SerialRts rts);
      int rtsDelay();
      bool setRtsDelay (int delay);

      static int baud (const std::string & settings);
      static char parity (const std::string & settings);
      static int stop (const std::string & settings);

      friend class Device;
    protected:
      class Private;
      RtuLayer (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (RtuLayer)
  };

}
/**
 *  @}
 */

/* ========================================================================== */
#endif /* MODBUSPP_H defined */
