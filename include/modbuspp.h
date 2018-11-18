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
        Byte,
        Word,
        LongWord,
        LongLongWord,
        Float,
        Double
      };
      
      enum Endian {
        EndianBigBig = 0x00, // bytes in big endian order, word in big endian order
        EndianBig = EndianBigBig,
        EndianBigLittle = 0x01,
        EndianLittleBig = 0x02,
        EndianLittleLittle = 0x03,
        EndianLittle = EndianLittleLittle
      };

      Data (Endian bigEndian = EndianBig);
      Data (char i, Endian bigEndian = EndianBig);
      Data (int i, Endian bigEndian = EndianBig);
      Data (long i, Endian bigEndian = EndianBig);
      Data (long long i, Endian bigEndian = EndianBig);
      Data (uint8_t i, Endian bigEndian = EndianBig);
      Data (uint16_t i, Endian bigEndian = EndianBig);
      Data (uint32_t i, Endian bigEndian = EndianBig);
      Data (uint64_t i, Endian bigEndian = EndianBig);
      Type type() const;
      size_t size() const;
  };

  /**
   * @class Timeout
   * @brief
   */
  class Timeout {
    public:
      Timeout (uint32_t s = 0, uint32_t us = 0) :
        sec (s), usec (us) {}
      uint32_t sec;
      uint32_t usec;
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

  /**
   * @class Device
   * @brief
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
   * @brief
   */
  class Master : public Device {
    public:

      Master (Net net = Tcp, const std::string & connection = "*",
              const std::string & settings = "502");
      virtual ~Master();

      int readDiscreteInputs (int addr, bool * dest, int nb = 1);
      int readCoils (int addr, bool * dest, int nb = 1);
      int readInputRegistrers (int addr, uint16_t * dest, int nb = 1);
      int readRegistrers (int addr, uint16_t * dest, int nb = 1);
      int writeCoil (int addr, bool value);
      int writeCoils (int addr, const bool * src, int nb);
      int writeRegistrer (int addr, uint16_t value);
      int writeRegistrers (int addr, const uint16_t * src, int nb);
      int writeReadRegistrers (int waddr, const uint16_t * src, int wnb,
                               int raddr, uint16_t * dest, int rnb);

    protected:
      class Private;
      Master (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Master)
  };

}
/**
 *  @}
 */

/* ========================================================================== */
#endif /* MODBUSPP_H defined */
