// libmodbuspp template
// This example code is in the public domain.
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string host ("127.0.0.1");
  string port = "1502";
  uint16_t t[] = { 12, 33, 0};

  if (argc > 1) {

    host = argv[1]; // the host can be provided as a parameter on the command line.
  }

  Server srv (Tcp, host, port); // new slave on TCP
  srv.setDebug();
  srv.setRecoveryLink();

  BufferedSlave & slv = srv.addSlave (10);
  slv.setBlock (InputRegister, 3);
  slv.writeInputRegisters (1, t, 3);

  if (srv.open ()) { // open a connection
    // success, do what you want here
    for (;;) {
      srv.task();
    }
    srv.close();
  }

  return 0;
}
/* ========================================================================== */
