// libmodbuspp template
// This example code is in the public domain.
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {

  RtuLayer net("/dev/ttyUSB0", "19200E1"); // Setting up rtu layer
  // TcpLayer net; // Setting up TCP layer for all interfaces on port 502
  Master mb (net); // new master on RTU

  if (mb.open (8)) { // open a connection to the slave at address 8
    // success, do what you want here
    // ....
    mb.close();
  }

  return 0;
}
/* ========================================================================== */
