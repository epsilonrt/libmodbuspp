// libmodbuspp template
// This example code is in the public domain.
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string port ("/dev/ttyUSB0");

  if (argc > 1) {

    port = argv[1]; // the serial port can be provided as a parameter on the command line.
  }

  Master mb (Rtu, port, "38400E1"); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);

  if (mb.open ()) { // open a connection
    // success, do what you want here
    uint16_t value;

    Slave & slv = mb.addSlave (33); // to the slave at address 33
    slv.readInputRegisters (1, &value);
    // ....
    mb.close();
  }

  return 0;
}
/* ========================================================================== */
