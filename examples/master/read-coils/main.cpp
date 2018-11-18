// libmodbuspp master read-coils example

// reads 5 coils on RTU slave at address 8

// This example code is in the public domain.
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string dev = string ("/dev/ttyUSB0");

  if (argc > 1) {
    dev = argv[1];
  }

  RtuLayer net (dev, "19200E1"); // Setting up rtu layer
  // TcpLayer net; // Setting up TCP layer for all interfaces on port 502
  Master mb (net); // new master on RTU

  if (mb.open (8)) { // open a connection to the slave at address 8
    bool coil[5];

    int ncoils = mb.readCoils (1, coil, 5); // reads coils 1 to 5
    if (ncoils > 0) {
      // if success, print the binary values
      cout << "coils: ";
      for (int i = 0; i < ncoils; i++) {
        
        cout << coil[i];
      }
      cout << endl;
    }
    mb.close();
  }

  return 0;
}
/* ========================================================================== */
