// Reads input registers of SolarPi pressure meter

// This example code is in the public domain.
#include <iostream>
#include <string>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;


// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string port ("/dev/ttyUSB0");

  if (argc > 1) {

    port = argv[1]; // the serial port can be provided as a parameter on the command line.
  }

  Master mb (Rtu, port , "38400E1"); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);

  Slave & slv = mb.addSlave (33); // SolarPi Pressure meter

  cout << "Reads input registers of slave[" << slv.number() << "] on " <<
       mb.connection() << " (" << mb.settings() << ")" << endl;

  if (mb.open ()) { // open a connection
    // success, do what you want here
    uint16_t values[2];

    if (slv.readInputRegisters (1, values, 2) == 2) {

      cout << "R0=" << values[0] << endl;
      cout << "R1=" << values[1] << endl;
    }
    else {
      cerr << "Unable to read input registers ! "  << mb.lastError() << endl;
      exit (EXIT_FAILURE);
    }
    mb.close();
  }
  else {
    cerr << "Unable to open MODBUS connection to " << port << " : " << mb.lastError() << endl;
    exit (EXIT_FAILURE);
  }

  return 0;
}
/* ========================================================================== */
