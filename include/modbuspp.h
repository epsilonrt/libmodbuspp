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
#include "global.h"

namespace Modbus {

  const int Broadcast = 0;

  enum Endian {
    EndianBigBig = 0x00, // bytes in big endian order, word in big endian order
    EndianBig = EndianBigBig,
    EndianBigLittle = 0x01,
    EndianLittleBig = 0x02,
    EndianLittleLittle = 0x03,
    EndianLittle = EndianLittleLittle
  };

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

  class Timeout {
    public:
      Timeout (uint32_t s = 0, uint32_t us = 0) :
        sec (s), usec (us) {}
      uint32_t sec;
      uint32_t usec;
  };

  class DataLinkLayer {
    public:
      enum Type {
        Rtu = 0,
        Tcp,
        Unknown = -1
      };
      DataLinkLayer (const DataLinkLayer & rhs) :
        host (rhs.host), settings (rhs.settings) {}
      DataLinkLayer () {}
      DataLinkLayer (const std::string & h, const std::string & s) :
        host (h), settings (s) {}
      virtual ~DataLinkLayer() {}
      virtual Type type() const {
        return Unknown;
      }
      std::string host;
      std::string settings;
  };

  class TcpLayer : public DataLinkLayer {
    public:
      TcpLayer (const std::string & tcpHost = "*", const std::string & tcpService = "502") :
        DataLinkLayer (tcpHost, tcpService) {}
      TcpLayer (const DataLinkLayer & rhs) :
        DataLinkLayer (rhs) {}
      Type type() const {
        return Tcp;
      }
      const char * node() const;
      const char * service() const;
  };

  class RtuLayer: public DataLinkLayer {
    public:
      RtuLayer (const std::string & rtuDevice, const std::string & rtuSettings = "19200E1") :
        DataLinkLayer (rtuDevice, rtuSettings) {}
      RtuLayer (const DataLinkLayer & rhs) :
        DataLinkLayer (rhs) {}
      Type type() const {
        return Rtu;
      }
      const char * device() const;
      int baud() const;
      char parity() const;
      int stop() const;
  };

  class Device  {
    public:

      Device (const DataLinkLayer & sublayer = DataLinkLayer());
      Device (const Device & other);
      virtual ~Device();

      void swap (Device &other);
      Device& operator= (const Device &other);

      virtual bool open();
      virtual void close();
      bool flush();
      bool isOpen() const;

      bool setResponseTimeout (const Timeout & timeout);
      bool responseTimeout (Timeout & timeout);
      bool setByteTimeout (const Timeout & timeout);
      bool byteTimeout (Timeout & timeout);

      bool setDebug (bool debug = true);
      
      const DataLinkLayer &  dataLinkLayer() const;
      bool isNull() const;
      const std::string & error() const;

      void setPduAdressing (bool pduAdressing = true);
      bool pduAdressing() const;

    protected:
      class Private;
      Device (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Device)
  };

  class Master : public Device {
    public:
      static const int TcpSlave = 0xFF;

      Master (const DataLinkLayer & sublayer = DataLinkLayer());
      virtual ~Master();

      bool open (int slave = TcpSlave);
      int slave() const;

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
