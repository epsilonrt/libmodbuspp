/**
 * @brief libmodbuspp simple-server-json example
 *
 * Shows how to make a purely virtual MODBUS TCP server, the JSON file describes 
 * the different data blocks (coils, discrete inputs, input and storage registers) 
 * and initializes the values at startup.
 *
 * @code
 * simple-server-json ../tcp-server.json
 * @endcode
 *
 * Once the server has started you can test it with mbpoll :
 *
 * @code
    $ mbpoll -mtcp -p1502 -a10 -t3 -c8 localhost
    mbpoll 1.4-12 - FieldTalk(tm) Modbus(R) Master Simulator
    Copyright © 2015-2019 Pascal JEAN, https://github.com/epsilonrt/mbpoll
    This program comes with ABSOLUTELY NO WARRANTY.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type 'mbpoll -w' for details.

    Protocol configuration: Modbus TCP
    Slave configuration...: address = [10]
                            start reference = 1, count = 8
    Communication.........: localhost, port 1502, t/o 1.00 s, poll rate 1000 ms
    Data type.............: 16-bit register, input register table

    -- Polling slave 10... Ctrl-C to stop)
    [1]:  37
    [2]:  40
    [3]:  15
    [4]:  28
    [5]:  11
    [6]:  2019
    [7]:  4
    [8]:  332
 * @endcode
 *
 */
#include <csignal>
#include <thread>
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

Server srv; // instantiates new MODBUS Server

// -----------------------------------------------------------------------------
// Signal trap, triggers during a CTRL+C or if kill is called
void
sighandler (int sig) {

  srv.close();
  cout << "everything was closed." << endl << "Have a nice day !" << endl;
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {

  if (argc < 2) {

    cerr << "Error: the JSON filename must be provided as a parameter on the command line !" << endl;
    cerr << "e.g. : " << argv[0] << " virtual-server-tcp.json" << endl;
    exit (EXIT_FAILURE);
  }

  string  jsonfile = argv[1];

  cout << "Simple Server" << endl;
  // CTRL+C and kill call triggers the trap sighandler()
  signal (SIGINT, sighandler);
  signal (SIGTERM, sighandler);
  cout << "Press CTRL+C to stop... " << endl << endl;

  try {
    cout << "opening " << jsonfile << "..." << endl;
    srv.setConfig (jsonfile, "modbuspp-server");

    if (srv.open ()) { // open a connection
      cout << "Listening server on " <<
           srv.connection() << ":" << srv.settings() << "..." << endl << endl;

      srv.run();
      while (srv.isOpen()) {

        // std::this_thread::yield();
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
      }
    }
  }
  catch (std::exception & e) {

    cerr << "Error: " << e.what() << endl;
  }
  catch (...) {

    cerr << "Unattended exception !" << endl;
  }

  return EXIT_FAILURE;
}
/* ========================================================================== */
