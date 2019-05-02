// Reads the slave identifier from SolarPi humidity meter and print as follow :

// Length: 14
// Id    : 0x02
// Status: On
// Data  : press-1.1.58

// This example code is in the public domain.
#include <iostream>
#include  <iomanip>
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
    SlaveId<uint8_t> i; 

    mb.setSlave (33); // to the slave at address 33
    if (mb.reportSlaveId (i) > 0) {

      cout << "Length: " << i.size() << endl;
      cout << "Id    : 0x" << setfill('0') << setw(2) << hex << (int) i.id() << endl;
      cout << "Status: " << (i.status() ? "On" : "Off") << endl;
      cout << "Data  : " << i.data() << endl;
    }
    else {
      cerr << "Unable to read slave identifier ! "  << mb.lastError() << endl;
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
