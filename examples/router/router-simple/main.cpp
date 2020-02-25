/**
 * @brief libmodbuspp router-simple example
 *
  This example shows how to create a MODBUS TCP-RTU router that relays requests
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
#include <csignal>
#include <thread>
#include <iostream>
#include <modbuspp.h>
#include <modbuspp/popl.h>

using namespace std;
using namespace Modbus;

Router router; // instantiates new MODBUS Router

// -----------------------------------------------------------------------------
// Signal trap, triggers during a CTRL+C or if kill is called
void
sighandler (int sig) {

  router.close();
  cout << "everything was closed." << endl << "Have a nice day !" << endl;
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string host;
  string port;
  string serial;
  string settings;

  // parsing options from the command line
  // watch https://github.com/badaix/popl to understand how we use popl ...
  popl::OptionParser CmdLine ("Allowed options");
  auto help_option = CmdLine.add<popl::Switch> ("h", "help", "produce help message");
  CmdLine.add<popl::Value<string>> ("H", "host", "listening address of the server", "127.0.0.1", &host);
  CmdLine.add<popl::Value<string>> ("p", "port", "server listening port", "1502", &port);
  CmdLine.add<popl::Value<string>> ("P", "serial", "serial port", "/dev/ttyUSB0", &serial);
  CmdLine.add<popl::Value<string>> ("s", "settings", "serial port settings", "38400E1", &settings);
  auto rs485_option = CmdLine.add<popl::Switch> ("R", "rs485", "RS-485 mode (/RTS on (0) after sending)");

  cout << "--- Modbus Router ---" << endl;
  // CTRL+C and kill call triggers the trap sighandler()
  signal (SIGINT, sighandler);
  signal (SIGTERM, sighandler);
  cout << "Press CTRL+C to stop... " << endl;

  try {
    CmdLine.parse (argc, argv);

    // print auto-generated help message then exit
    if (help_option->count() == 1) {
      cout << CmdLine << endl;
      exit (EXIT_SUCCESS);
    }

    router.setBackend (Tcp, host, port);
    // Enabling debug mode to display transmitted and received frames
    router.setDebug();
    // Activation of automatic reconnection mode
    // If this option is enabled, the server is restarted after each
    // disconnection of a client over TCP.
    router.setRecoveryLink();

    // Creating the MODBUS master link that controls the serial link
    Master & inside = router.addMaster ("inside", Rtu, serial, settings);
    inside.setDebug();
    if (rs485_option->count() == 1) {

      inside.rtu().setRts (RtsDown);
      inside.rtu().setSerialMode (Rs485);
    }

    // Declaration of a slave with the address 33 accessible via the TCP server
    // and physically connected to the serial link managed by inside
    // Be sure to set press as a reference (&) because the BufferedSlave
    // variable itself is owned by the router.
    BufferedSlave & press = router.addSlave (33, &inside);

    // Declaration of the input register block of the press slave, it has
    // 6 registers, of which the following description is :
    // @ Reg.  Size    Sensor  Description
    // 1       16-bit  MPX     Pressure measured in tenths of hPa, unsigned integer
    // 2       16-bit  HSC     Pressure measured in tenths of hPa, unsigned integer
    // 3       16-bit  HSC     Temperature measured in hundredths of °C, signed integer
    // 4       16-bit  MPX     Raw ADC value of pressure in LSB, unsigned integer
    // 5       16-bit  HSC     HSC raw value of pressure in LSB, unsigned integer
    // 6       16-bit  HSC     HSC raw value of temperature in LSB, unsigned integer
    press.setBlock (InputRegister, 6);
    // Declaration of the block of holding registers of the press slave,
    // it comprises 4 registers 32-bit, thus 8 registers 16-bit, here is
    // the description :
    // @ Reg.  Size    Sensor  Description                                  Default
    // 1       32-bit  MPX     Minimum standard pressure, float in hPa      150
    // 3       32-bit  MPX     Maximum standard pressure, float in hPa      1150
    // 5       32-bit  MPX     Minimum standard ADC value, float in LSB ADC 40.96
    // 7       32-bit  MPX     Maximum ADC standard value, float in LSB ADC 962.56
    press.setBlock (HoldingRegister, 8); // 32-bit * 4

    // We do the same thing with the humidity sensor at address 32, this slave
    // has only 2 input registers.
    BufferedSlave & hum = router.addSlave (32, &inside);
    hum.setBlock (InputRegister, 2);
    hum.setBlock (HoldingRegister, 8); // 32-bit * 4

    if (router.open ()) { // open a connection
      cout << "Listening server on " <<
           router.connection() << ":" << router.settings() << "..."
           << endl << endl;

      router.run();
      while (router.isOpen()) {

        //std::this_thread::yield();
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
      }
    }
  }
  catch (std::logic_error & e) {

    cerr << "Logic error: " << e.what() << endl;
  }
  catch (std::runtime_error & e) {

    cerr << "Runtime error: " << e.what() << endl;
  }
  catch (...) {

    cerr << "Unattended exception !" << endl;
  }

  return 0;
}
/* ========================================================================== */
