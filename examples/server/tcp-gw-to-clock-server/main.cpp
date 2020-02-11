/**
  @brief MODBUS TCP gateway for clock-server example through serial port (RTU)

  This example shows how to create a MODBUS TCP-RTU gateway that relays requests
  to slaves connected on a serial link.
  Here the TCP gateway server listens on IPv4 localhost on port 1502 and relays
  requests to the serial link /dev/ttyUSB0. These settings can be changed
  through the command line option line:

    Allowed options:
      -h, --help                        produce help message
      -H, --host arg (=127.0.0.1)       listening address of the server
      -p, --port arg (=1502)            server listening port
      -P, --serial arg (=/dev/ttyUSB0)  serial port where we speak
      -s, --settings arg (=38400E1)     serial port settings
      -R, --rs485                       RS-485 mode (/RTS on (0) after sending)

  In this example, the slave connected on the link are the clock-server example
  (id: 10).

  Once the server and tcp gateway has started you can test it with mbpoll :

  @code
    $ mbpoll -m tcp -p 1502 -a10 -t3 -c8 localhost
    mbpoll 1.4-12 - FieldTalk(tm) Modbus(R) Master Simulator
    Copyright © 2015-2019 Pascal JEAN, https://github.com/epsilonrt/mbpoll
    This program comes with ABSOLUTELY NO WARRANTY.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type 'mbpoll -w' for details.

    Protocol configuration: Modbus TCP
    Slave configuration...: address = [10]
                            start reference = 1, count = 8
    Communication.........: localhost, port 1502, t/o 1.00 s, poll rate 200 ms
    Data type.............: 16-bit register, input register table

    -- Polling slave 10... Ctrl-C to stop)
    [1]:  22
    [2]:  31
    [3]:  10
    [4]:  11
    [5]:  2
    [6]:  2020
    [7]:  2
    [8]:  42

  @endcode

  This example code is in the public domain.
*/
#include <iostream>
#include <modbuspp.h>
#include <modbuspp/popl.h>

using namespace std;
using namespace Modbus;

const int SlaveAddress = 10;

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string host;
  string port;
  string settings;
  string serial;

  // parsing options from the command line
  // watch https://github.com/badaix/popl to understand how we use popl ...
  popl::OptionParser CmdLine ("Allowed options");
  auto help_option = CmdLine.add<popl::Switch> ("h", "help", "produce help message");
  CmdLine.add<popl::Value<string>> ("H", "host", "listening address of the server", "127.0.0.1", &host);
  CmdLine.add<popl::Value<string>> ("p", "port", "server listening port", "1502", &port);
  CmdLine.add<popl::Value<string>> ("P", "serial", "serial port where we speak", "/dev/ttyUSB0", &serial);
  CmdLine.add<popl::Value<string>> ("s", "settings", "serial port settings", "38400E1", &settings);
  auto rs485_option = CmdLine.add<popl::Switch> ("R", "rs485", "RS-485 mode (/RTS on (0) after sending)");
  CmdLine.parse (argc, argv);
  // print auto-generated help message then exit
  if (help_option->count() == 1) {
    cout << CmdLine << endl;
    exit (0);
  }

  // Creating the MODBUS master link that controls the serial link
  Master mb (Rtu, serial , settings); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  if (rs485_option->count() == 1) {

    mb.rtu().setRts (RtsDown);
    mb.rtu().setSerialMode (Rs485);
  }

  // Enabling debug mode to display transmitted and received frames
  mb.setDebug();

  if (mb.open()) { // Opening the master link if successful, create the server

    // New TCP server listens on host and port
    Server srv (Tcp, host, port);

    // Enabling debug mode to display transmitted and received frames
    srv.setDebug();
    // Activation of automatic reconnection mode
    // If this option is enabled, the server is restarted after each
    // disconnection of a client over TCP.
    srv.setRecoveryLink();

    BufferedSlave & slv = srv.addSlave (SlaveAddress, &mb); // Adding a new slave to the server

    cout << "Slave id: " << slv.number() << endl << endl;
    if (srv.debug()) {
      cout << "Mapping of registers:" << endl
           << "--- Input Registers" << endl
           << "@ Reg.  Size    Description" << endl
           << "1       16-bit  Seconds (0-60), unsigned" << endl
           << "2       16-bit  Minutes (0-59), unsigned" << endl
           << "3       16-bit  Hours (0-23), unsigned" << endl
           << "4       16-bit  Day of the month (1-31), unsigned" << endl
           << "5       16-bit  Month (1-12), unsigned" << endl
           << "6       16-bit  Year e.g. 2019, unsigned" << endl
           << "7       16-bit  Day of the week (0-6, Sunday = 0), unsigned" << endl
           << "8       16-bit  Day in the year (1-366, 1 Jan = 1), unsigned" << endl
           << "--- Holding Registers" << endl
           << "@ Reg.  Size    Description" << endl
           << "1       32-bit  number of seconds to add to UTC to get local time, signed" << endl
           << "--- Coils" << endl
           << "@ Reg.  Size    Description" << endl
           << "1       1-bit   Daylight saving time" << endl << endl;
    }

    /* Mapping of registers :
      --- Input Registers
      @ Reg.  Size    Description
      1       16-bit  Seconds (0-60), unsigned
      2       16-bit  Minutes (0-59), unsigned
      3       16-bit  Hours (0-23), unsigned
      4       16-bit  Day of the month (1-31), unsigned
      5       16-bit  Month (1-12), unsigned
      6       16-bit  Year e.g. 2019, unsigned
      7       16-bit  Day of the week (0-6, Sunday = 0), unsigned
      8       16-bit  Day in the year (1-366, 1 Jan = 1), unsigned */
    slv.setBlock (InputRegister, 8);

    /* --- Holding Registers
     @ Reg.  Size    Description
     1       32-bit  number of seconds to add to UTC to get local time, signed */
    slv.setBlock (HoldingRegister, 2);

    /* --- Coils
     @ Reg.  Size    Description
     1       1-bit   Daylight saving time, true = summer time */
    slv.setBlock (Coil, 1);

    cout << "Modbus TCP Gateway for clock server" << endl << "listening on " <<
         srv.backend().connection() << ":" << srv.backend().settings() << endl;

    // after displaying the server elements, we start this one ...
    if (srv.open ()) {
      // if the startup is successful, we perform a polling loop that performs
      // all server operations:
      // - receive requests on TCP
      // - redirection of requests to the serial link
      // - recovery of information coming from the serial link and storage in a memory buffer
      // - response to the TCP client with its information
      for (;;) {
        srv.poll (1000);
      }
      srv.close();
    }

  }

  return 0;
}
/* ========================================================================== */
