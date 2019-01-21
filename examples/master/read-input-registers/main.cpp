// libmodbuspp template
// This example code is in the public domain.
#include <iostream>
#include <string>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;


// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string port ("/dev/ttyUSB1");

  Master mb (Rtu, port , "38400E1"); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);

  if (mb.open ()) { // open a connection
    // success, do what you want here
    uint16_t values[2];

    mb.setSlave (33); // to the slave at address 33
    if (mb.readInputRegisters (1, values, 2) == 2) {

      cout << "R0=" << values[0] << endl;
      cout << "R1=" << values[1] << endl;
    }
    else {
      cerr << "Unable to read input registers !"  << endl;
      exit (EXIT_FAILURE);
    }
    mb.close();
  }
  else {
    cerr << "Unable to open MODBUS connection to " << port << endl;
    exit (EXIT_FAILURE);
  }

  return 0;
}
/* ========================================================================== */
