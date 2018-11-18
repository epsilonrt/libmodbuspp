// libmodbuspp master read-coils example

// reads 5 coils on RTU slave at address 8

// This example code is in the public domain.
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

int main (int argc, char **argv) {
  string port = string ("/dev/ttyUSB0");

  if (argc > 1) {

    port = argv[1];
  }

  Master mb (Rtu, port, "19200E1"); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);

  if (mb.open ()) { // open a connection
    bool coil[5];

    mb.setSlave (8); // to the slave at address 8
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
