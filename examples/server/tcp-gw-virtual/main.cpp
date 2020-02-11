/**
  @brief MODBUS TCP-RTU gateway

  This example shows how to create a MODBUS TCP-RTU gateway that relays requests
  to slaves connected on a serial link.
  Here the TCP gateway server listens on IPv4 localhost on port 1502 and relays
  requests to the serial link / dev / ttyUSB0. These settings can be changed
  through the command line option line:

      Allowed options:
        -h, --help                        produce help message
        -H, --host arg (=127.0.0.1)       listening address of the server
        -p, --port arg (=1502)            server listening port
        -P, --serial arg (=/dev/ttyUSB0)  serial port
        -s, --settings arg (=38400E1)     serial port settings

  In this example, the slaves connected on the link are sensors of humidity
  (id: 32) and pressure (id: 33) of the SolarPi telemetry system.

  Once the server has started you can test it with mbpoll :

  @code
      $ mbpoll -a32,33 -mtcp -p1502 -t4:float -c4 -1 localhost
      mbpoll 1.4-12 - FieldTalk(tm) Modbus(R) Master Simulator
      Copyright © 2015-2019 Pascal JEAN, https://github.com/epsilonrt/mbpoll
      This program comes with ABSOLUTELY NO WARRANTY.
      This is free software, and you are welcome to redistribute it
      under certain conditions; type 'mbpoll -w' for details.

      Protocol configuration: Modbus TCP
      Slave configuration...: address = [32,33]
                              start reference = 1, count = 4
      Communication.........: localhost, port 1502, t/o 1.00 s, poll rate 1000 ms
      Data type.............: 32-bit float (little endian), output (holding) register table

      -- Polling slave 32...
      [1]:  0
      [3]:  75.3
      [5]:  173.2
      [7]:  650.49
      -- Polling slave 33...
      [1]:  150
      [3]:  1150
      [5]:  40.96
      [7]:  962.56
  @endcode

  This example code is in the public domain.
*/
#include <iostream>
#include <iomanip>
#include <modbuspp.h>
#include <modbuspp/popl.h>

using namespace std;
using namespace Modbus;

const int slaveAddr = 10;

// -----------------------------------------------------------------------------
// We define a class that simulates an underlying link
// In a real project, this class should send and receive messages on a real link (LoRa... ;-)
class MyVirtualLink {

  public:
    MyVirtualLink (uint8_t myAddress, Master & plug);
    bool poll();

  private:
    bool send (const Message & req, uint8_t dest);
    bool receiveTimeout (Message & rsp, uint16_t timeout, uint8_t * source = nullptr);

    uint8_t _myAddress;
    Master & _plug;
    Message _rsp;

    // These variables are dummy, they allow you to create a response based on the request
    std::vector<uint16_t> inputRegs;
    std::vector<uint16_t> holdingRegs;
    std::vector<bool> bits;
    std::vector<bool> coils;
};

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string host;
  string port;
  string settings;

  // parsing options from the command line
  // watch https://github.com/badaix/popl to understand how we use popl ...
  popl::OptionParser CmdLine ("Allowed options");
  auto help_option = CmdLine.add<popl::Switch> ("h", "help", "produce help message");
  CmdLine.add<popl::Value<string>> ("H", "host", "listening address of the server", "127.0.0.1", &host);
  CmdLine.add<popl::Value<string>> ("p", "port", "server listening port", "1502", &port);
  CmdLine.parse (argc, argv);
  // print auto-generated help message then exit
  if (help_option->count() == 1) {
    cout << CmdLine << endl;
    exit (0);
  }

  // Creating the MODBUS master link that controls the virtual RTU link
  Master mb (VirtualRtu, "virtual"); // new virtual RTU master connected to my link
  MyVirtualLink lnk (0xC0, mb);

  mb.setResponseTimeout (3); // set response timeout to 3 seconds

  // Enabling debug mode to display transmitted and received frames
  mb.setDebug();

  if (mb.open()) { // Opening the master link if successful, create the server

    // New TCP server listens on host and port
    Server srv (Tcp, host, port);
    srv.setResponseTimeout (5); // set response timeout to 5 seconds

    // Enabling debug mode to display transmitted and received frames
    srv.setDebug();
    // Activation of automatic reconnection mode
    // If this option is enabled, the server is restarted after each
    // disconnection of a client over TCP.
    srv.setRecoveryLink();

    // Declaration of a slave with the address 33 accessible via the TCP server
    // and physically connected to the serial link managed by mb
    // Be sure to set slave as a reference (&) because the BufferedSlave
    // variable itself is owned by the srv server.
    BufferedSlave & slave = srv.addSlave (33, &mb);

    // Declaration of the input register block of the slave slave, it has
    // 6 registers, of which the following description is :
    // @ Reg.  Size    Sensor  Description
    // 1       16-bit  MPX     Pressure measured in tenths of hPa, unsigned integer
    // 2       16-bit  HSC     Pressure measured in tenths of hPa, unsigned integer
    // 3       16-bit  HSC     Temperature measured in hundredths of °C, signed integer
    // 4       16-bit  MPX     Raw ADC value of pressure in LSB, unsigned integer
    // 5       16-bit  HSC     HSC raw value of pressure in LSB, unsigned integer
    // 6       16-bit  HSC     HSC raw value of temperature in LSB, unsigned integer
    slave.setBlock (InputRegister, 6);

    // Declaration of the block of holding registers of the slave slave,
    // it comprises 4 registers 32-bit, thus 8 registers 16-bit, here is
    // the description :
    // @ Reg.  Size    Sensor  Description                                  Default
    // 1       32-bit  MPX     Minimum standard pressure, float in hPa      150
    // 3       32-bit  MPX     Maximum standard pressure, float in hPa      1150
    // 5       32-bit  MPX     Minimum standard ADC value, float in LSB ADC 40.96
    // 7       32-bit  MPX     Maximum ADC standard value, float in LSB ADC 962.56
    slave.setBlock (HoldingRegister, 4); // 32-bit * 4

    cout << "Modbus TCP Gateway" << endl << "listening on " <<
         srv.backend().connection() << ":" << srv.backend().settings() << endl;

    // after displaying the server elements, we start this one ...
    if (srv.open ()) {
      // if the startup is successful, we start the daemon that performs
      // all server operations:
      // - receive requests on TCP
      // - redirection of requests to the serial link
      // - recovery of information coming from the serial link and storage in a memory buffer
      // - response to the TCP client with its information
      if (srv.start()) {

        while (srv.isRunning()) {

          lnk.poll();
        }
      }
      srv.close();
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
MyVirtualLink::MyVirtualLink (uint8_t myAddress, Master & plug) :
  _myAddress (myAddress), _plug (plug), _rsp (plug)  {

  if (_plug.net() != VirtualRtu) {

    throw std::invalid_argument ("MyVirtualLink only works with MODBUS masters on Virtual RTU.");
  }
  inputRegs = {1, 2, 3, 4, 5, 6 };
  holdingRegs = { 150, 1150, 40, 962 };
}

// -----------------------------------------------------------------------------
bool MyVirtualLink::poll() {

  if (_plug.isOpen()) {
    Message req (_plug);

    if (_plug.virtualRtu().pull (req)) { // a request has been received, it must be relayed to the link
      uint8_t dest;

      // retrieves the destination address from the modbus address, here the
      // address is stored in the 4 most significant bits.
      dest = req.slave() & 0xF0;

      // We send the request on the dummy link
      if (send (req, dest)) {
        Message rsp (_plug);
        uint8_t src;

        // we are waiting for a response on the dummy link
        if (receiveTimeout (rsp, _plug.responseTimeout() * 1000, &src)) {

          if (src == dest) {

            return _plug.virtualRtu().push (rsp);
          }
        }
      }
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
bool MyVirtualLink::send (const Message & req, uint8_t dest) {

  uint16_t crcIndex = 0;
  size_t s = req.startingAddress();
  size_t q = req.quantity();

  _rsp = req;

  // Here we simulate a slave connected on the link
  switch (req.function()) {

    case ReadInputRegisters:
      if ( (s + q - 1) > inputRegs.size()) {

        _rsp.setException (IllegalDataAddress);
      }
      else {

        _rsp.setByteCount (q * 2);
        _rsp.setRegisterValues (s, q, &inputRegs[s]);
        // incrementing input register values (to see something change)
        for (size_t i = 0; i < q; i++) {
          inputRegs[s + i]++;
        }
      }
      break;

    case ReadHoldingRegisters:
      if ( (s + q - 1) > holdingRegs.size()) {

        _rsp.setException (IllegalDataAddress);
      }
      else {

        _rsp.setByteCount (q * 2);
        _rsp.setRegisterValues (s, q, &holdingRegs[s]);
      }
      break;

    case WriteMultipleRegisters:
      if ( (s + q - 1) > holdingRegs.size()) {

        _rsp.setException (IllegalDataAddress);
      }
      else {
        _rsp.registerValues (s, q, &holdingRegs[s]);
        crcIndex = 5;
      }
      break;

    default:
      _rsp.setException (IllegalFunction);
      break;
  }

  _rsp.setCrc (crcIndex);
  return true;
}

// -----------------------------------------------------------------------------
bool MyVirtualLink::receiveTimeout (Message & rsp, uint16_t timeout, uint8_t * source) {

  if (source) {

    *source = _rsp.slave() & 0xF0;
  }

  if (_rsp.size()) {

    rsp = _rsp;
    _rsp.clear();
    return true;
  }
  return false;
}
/* ========================================================================== */
