// libmodbuspp template
// This example code is in the public domain.
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {

  Master mb (Rtu, port, "19200E1"); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);

  if (mb.open ()) { // open a connection
    // success, do what you want here
    uint16_t value;

    mb.setSlave (8); // to the slave at address 8
    mb.readInputRegistrers (1, &value);
    // ....
    mb.close();
  }

  return 0;
}
/* ========================================================================== */
