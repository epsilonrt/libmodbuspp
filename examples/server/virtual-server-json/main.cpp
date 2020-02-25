/**
 * @brief libmodbuspp clock-server-json example
 *
 * Shows how to use libmodbuspp to build a MODBUS time server.
 * the MODBUS server is configured from the JSON ./server.json file
 *
 * @code
 * clock-server-json json_filename
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
 * These data correspond to 15:40:37 on Thursday 28/11/2019, 332nd day of the
 * year. To read the time difference from GMT (in seconds) :
 *
 * @code
    $ mbpoll -mtcp -p1502 -a10 -t4:int -B -1 localhost
      ....
      -- Polling slave 10...
      [1]:  3600
 * @endcode
 *
 * To set the offset to GMT-2 hours :
 *
 * @code
    $ mbpoll -mtcp -p1502 -a10 -t4:int -B localhost -- -7200
      ....
      Written 1 references.
 * @endcode
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
int messageHandler (Message * req, Device * dev) {

  cout << "Receive message, size : " << req->aduSize() << endl;
  if (req->function() == ReadInputRegisters) {
    // the dummy word will be the returned value, incremented after each reading
    static uint16_t dummy = 1; 
    
    // get request parameters
    uint16_t N = req->quantity();
    uint16_t index = req->startingAddress();
    
    // build response, see page 16 of MODBUS Application Protocol Specification
    Response rsp (*req); // uses copy constructor to keep transaction id
    rsp.setSize(1); // keep until function code
    rsp.setByteCount (N * 2);
    for (uint16_t i = 0; i < N; i++) {

      rsp.setRegisterValue (index + i, dummy++);
    }

    return dev->sendRawMessage (rsp, true);
  }
  return 0;
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {

  if (argc < 2) {

    cerr << "Error: the JSON filename must be provided as a parameter on the command line !" << endl;
    cerr << "e.g. : " << argv[0] << " null-server-tcp.json" << endl;
    exit (EXIT_FAILURE);
  }

  string  jsonfile = argv[1];

  cout << "Null Server" << endl;
  // CTRL+C and kill call triggers the trap sighandler()
  signal (SIGINT, sighandler);
  signal (SIGTERM, sighandler);
  cout << "Press CTRL+C to stop... " << endl << endl;

  try {
    cout << "opening " << jsonfile << "..." << endl;
    srv.setConfig (jsonfile, "modbuspp-server");
    srv.setMessageCallback (messageHandler);

    if (srv.open ()) { // open a connection
      cout << "Listening server on " <<
           srv.connection() << ":" << srv.settings() << "..." << endl << endl;

      srv.run();
      while (srv.isOpen()) {

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

  return EXIT_FAILURE;
}
/* ========================================================================== */
