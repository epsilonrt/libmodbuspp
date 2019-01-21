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

#ifndef MODBUSPP_H
#define MODBUSPP_H

#include <string>
#include <modbus.h>
#include "global.h"

/**
 *
 */
namespace Modbus {

  const int Broadcast = MODBUS_BROADCAST_ADDRESS;
  const int TcpSlave = MODBUS_TCP_SLAVE;
  const int Unknown = -1;

  enum Net {
    Rtu,
    Tcp,
    NoNet = Unknown
  };

  enum SerialMode {
    Rs232 = MODBUS_RTU_RS232,
    Rs485 = MODBUS_RTU_RS485,
    UnknownMode = Unknown
  };

  enum SerialRts {
    RtsNone = MODBUS_RTU_RTS_NONE,
    RtsUp = MODBUS_RTU_RTS_UP,
    RtsDown = MODBUS_RTU_RTS_DOWN,
    UnknownRts = Unknown
  };

  /**
   * @class Data
   * @brief
   */
  class Data {
    public:
      enum Type {
        Byte, // 2 Bytes (nothing is smaller than the word on MODBUS)
        Word, // 2 Bytes - 1 MODBUS register
        LongWord, // 4 Bytes - 2 MODBUS registers
        LongLongWord, // 8 Bytes - 4 MODBUS registers
        Float, // 4 Bytes - 2 MODBUS registers
        Double, // 8 Bytes - 4 MODBUS registers
        Void = -1
      };

      enum Endian { // network number ABCD
        EndianBigBig = 0x00,    // bytes in big endian order, word in big endian order : ABCD
        EndianBig = EndianBigBig, // big endian order : ABCD
        EndianBigLittle = 0x01, // bytes in big endian order, word in little endian order : CDAB
        EndianLittleBig = 0x02, // bytes in little endian order, word in big endian order : BADC
        EndianLittleLittle = 0x03, // bytes in little endian order, word in little endian order : DCBA
        EndianLittle = EndianLittleLittle // little endian order : DCBA
      };

      Data (Type type = Void, Endian endian = EndianBig); // default constructor
      Data (const Data & other);
      virtual ~Data();

      Data (uint8_t v, Endian endian = EndianBig);
      Data (uint16_t v, Endian endian = EndianBig);
      Data (uint32_t v, Endian endian = EndianBig);
      Data (uint64_t v, Endian endian = EndianBig);
      Data (char v, Endian endian = EndianBig);
      Data (int v, Endian endian = EndianBig);
      Data (long v, Endian endian = EndianBig);
      Data (long long v, Endian endian = EndianBig);
      Data (float v, Endian endian = EndianBig);
      Data (double v, Endian endian = EndianBig);

      void swap (Data &other);
      Data& operator= (const Data &other);

      void set (uint8_t v);
      void set (uint16_t v);
      void set (uint32_t v);
      void set (uint64_t v);

      void set (char v);
      void set (int v);
      void set (long v);
      void set (long long v);
      void set (float v);
      void set (double v);

      void get (uint8_t & v) const;
      void get (uint16_t & v) const;
      void get (uint32_t & v) const;
      void get (uint64_t & v) const;

      void get (char & v) const;
      void get (int & v) const;
      void get (long & v) const;
      void get (long long & v) const;
      void get (float & v) const;
      void get (double & v) const;

      Endian endianness() const;
      Type type() const;
      void setType (Type t);
      size_t size() const;
      uint16_t * ptr();
      const uint16_t * ptr() const;

      // debug purpose
      void print () const;
      static void print (const uint8_t * p, const size_t s);
      static void print (const uint8_t & v);
      static void print (const uint16_t & v);
      static void print (const uint32_t & v);
      static void print (const uint64_t & v);
      static void print (const char & v);
      static void print (const int & v);
      static void print (const long & v);
      static void print (const long long & v);
      static void print (const float & v);
      static void print (const double & v);

    protected:
      class Private;
      Data (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Data)
  };

  /**
   * @class Timeout
   * @brief Représente une durée de timeout
   */
  class Timeout {
    public:
      Timeout (uint32_t s = 0, uint32_t us = 0) :
        sec (s), usec (us) {}
      uint32_t sec;
      uint32_t usec;
  };


  class RtuLayer;
  class TcpLayer;

  /**
   * @class Device
   * @brief Représente un équipement connecté à MODBUS
   */
  class Device  {
    public:

      Device (Net net = Tcp, const std::string & connection = "*",
              const std::string & settings = "502");
      Device (const Device & other);
      virtual ~Device();

      void swap (Device &other);
      Device& operator= (const Device &other);

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
   * @brief MODBUS Master (Client)
   */
  class Master : public Device {

    public:
      /**
       * @brief Constructor
       * @param net
       * @param connection
       * @param settings
       */
      Master (Net net = Tcp, const std::string & connection = "*",
              const std::string & settings = "502");
      /**
       * @brief Destructor
       */
      virtual ~Master();

      /**
       * @brief
       * @param addr
       * @param dest
       * @param nb
       * @return
       */
      int readDiscreteInputs (int addr, bool * dest, int nb = 1);
      /**
       * @brief
       * @param addr
       * @param dest
       * @param nb
       * @return
       */
      int readCoils (int addr, bool * dest, int nb = 1);
      /**
       * @brief
       * @param addr
       * @param dest
       * @param nb
       * @return
       */
      int readInputRegisters (int addr, uint16_t * dest, int nb = 1);
      /**
       * @brief
       * @param addr
       * @param dest
       * @param nb
       * @return
       */
      int readRegistrers (int addr, uint16_t * dest, int nb = 1);
      /**
       * @brief write a single bit
       *
       * This function shall write the status of value at the address addr of
       * the remote device. \n
       * The function uses the Modbus function code 0x05 (force single coil).
       * @param addr
       * @param value
       * @return
       */
      int writeCoil (int addr, bool value);
      /**
       * @brief
       * @param addr
       * @param src
       * @param nb
       * @return
       */
      int writeCoils (int addr, const bool * src, int nb);
      /**
       * @brief
       * @param addr
       * @param value
       * @return
       */
      int writeRegistrer (int addr, uint16_t value);
      /**
       * @brief
       * @param addr
       * @param src
       * @param nb
       * @return
       */
      int writeRegistrers (int addr, const uint16_t * src, int nb);
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
      int writeReadRegistrers (int waddr, const uint16_t * src, int wnb,
                               int raddr, uint16_t * dest, int rnb);

    protected:
      class Private;
      Master (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Master)
  };


  /**
   * @class NetLayer
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
