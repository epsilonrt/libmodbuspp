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

#include <cstdint>
#include <cstddef>
#include <cerrno>
#include <sys/types.h>
#include <modbus.h>
#include <modbuspp/pimp.h>

/**
 * @brief libmodbuspp namespace
 */
namespace Modbus {

  const int Broadcast = MODBUS_BROADCAST_ADDRESS; ///< Modbus Broadcast Address
  const int TcpSlave = MODBUS_TCP_SLAVE; ///< Can be used in TCP mode to restore the default value
  const int Unknown = -1; ///< Value corresponding to an unknown parameter
  const uint16_t MaxPduLength = MODBUS_MAX_PDU_LENGTH; ///< maximum size of a PDU
  const uint8_t ExceptionFlag = 0x80;

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
   * Request To Send mode to communicate on a RS485 serial bus.
   *
   * By default, the mode is set to @b RtsNone and no signal is issued before
   * writing data on the wire.
   *
   * To enable the RTS mode, the values @b RtsUp or @b RtsDown must be used,
   * these modes enable the RTS mode and set the polarity at the same time.
   *
   * When @b RtsUp is used, an ioctl call is made with RTS flag enabled then
   * data is written on the bus after a delay of 1 ms, then another ioctl call
   * is made with the RTS flag disabled and again a delay of 1 ms occurs.
   *
   * The @b RtsDown mode applies the same procedure but with an inverted RTS flag.
   */
  enum SerialRts {
    RtsNone = MODBUS_RTU_RTS_NONE, ///< no use of the RTS.
    RtsUp = MODBUS_RTU_RTS_UP, ///< RTS flag ON during communication, OFF outside.
    RtsDown = MODBUS_RTU_RTS_DOWN, ///< RTS flag OFF during communication, ON outside.
    UnknownRts = Unknown   ///< Unknown RTS mode.
  };

  /**
   * @enum Endian
   * @brief Sequential order in which bytes are arranged
   */
  enum Endian {
    EndianBigBig = 0x00,    ///< Bytes in big endian order, word in big endian order : ABCD
    EndianBig = EndianBigBig, ///< Big endian order : ABCD
    EndianBigLittle = 0x01, ///< Bytes in big endian order, word in little endian order : CDAB
    EndianLittleBig = 0x02, ///< Bytes in little endian order, word in big endian order : BADC
    EndianLittleLittle = 0x03, ///< Bytes in little endian order, word in little endian order : DCBA
    EndianLittle = EndianLittleLittle ///< Little endian order : DCBA
  };
  
  /**
   * @enum Table
   * @brief Enumerations of data types managed by MODBUS
   */
  enum Table {
    DiscreteInput = 0,
    Coil = 1,
    InputRegister = 3,
    HoldingRegister = 4
  };

  /**
   * @enum Function
   * @brief Enumerations of MODBUS functions managed by libmodbuspp
   */
  enum Function {
    ReadCoils = MODBUS_FC_READ_COILS,
    ReadDiscreteInputs = MODBUS_FC_READ_DISCRETE_INPUTS,
    ReadHoldingRegisters = MODBUS_FC_READ_HOLDING_REGISTERS,
    ReadInputRegisters = MODBUS_FC_READ_INPUT_REGISTERS,
    WriteSingleCoil = MODBUS_FC_WRITE_SINGLE_COIL,
    WriteSingleRegister = MODBUS_FC_WRITE_SINGLE_REGISTER,
    ReadExceptionStatus = MODBUS_FC_READ_EXCEPTION_STATUS,
    // Diagnostic = 8, // Not implemented
    // GetComEventCounter = 11, // Not implemented
    // GetComEventLog = 12, // Not implemented
    WriteMultipleCoils = MODBUS_FC_WRITE_MULTIPLE_COILS,
    WriteMultipleRegisters = MODBUS_FC_WRITE_MULTIPLE_REGISTERS,
    ReportServerId = MODBUS_FC_REPORT_SLAVE_ID,
    // ReadFileRecord = 20, // Not implemented
    // WriteFileRecord = 21, // Not implemented
    MaskWriteRegister = MODBUS_FC_MASK_WRITE_REGISTER,
    ReadWriteMultipleRegisters = MODBUS_FC_WRITE_AND_READ_REGISTERS,
    // ReadFifoQueue = 24, // Not implemented
    // ReadDeviceId = 43, // Not implemented
    UnknownFunction = Unknown
  };

  /**
   * @enum ExceptionCode
   * @brief Enumerations of MODBUS exception code managed by libmodbuspp
   */
  enum ExceptionCode {
    IllegalFunction = MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
    IllegalDataAddress = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    IllegalDataValue = MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
    SlaveOrServerFailure = MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    Acknowledge = MODBUS_EXCEPTION_ACKNOWLEDGE,
    SlaveOrServerBusy = MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    NegativeAcknowledge = MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    MemoryParity = MODBUS_EXCEPTION_MEMORY_PARITY,
    NotDefined = MODBUS_EXCEPTION_NOT_DEFINED,
    GatewayPath = MODBUS_EXCEPTION_GATEWAY_PATH,
    GatewayTarget = MODBUS_EXCEPTION_GATEWAY_TARGET,
    ExceptionMax = MODBUS_EXCEPTION_MAX
  };
}
/* ========================================================================== */
