// libmodbuspp template
// This example code is in the public domain.
#include <csignal>
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

Master mb; // instantiate new MODBUS Master

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string port ("/dev/ttyUSB0");

  if (argc > 1) {

    port = argv[1]; // the serial port can be provided as a parameter on the command line.
  }

  mb.setBackend (Rtu, port, "38400E1"); // set master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);
  Slave & slv = mb.addSlave (33); // to the slave at address 33

  if (mb.open ()) { // open a connection
    // success, do what you want here
    uint16_t value;

    slv.readInputRegisters (1, &value);
    // ....
    cout << "R0=" << value << endl;
    mb.close();
  }

  return 0;
}
/* ========================================================================== */
