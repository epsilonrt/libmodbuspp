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

    port = argv[1]; // the serial port can be provided as a parameter on the command line.
  }

  Master mb (Rtu, port, "19200E1"); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);

  Slave slv (mb.addSlave (8));

  cout << "Reads coils of slave[" << slv.slave() << "] on " <<
       mb.backend().connection() << " (" << mb.backend().settings() << ")" << endl;

  if (mb.open ()) { // open a connection
    bool coil[5];

    int ncoils = slv.readCoils (1, coil, 5); // reads coils 1 to 5
    if (ncoils > 0) {

      // if success, print the binary values
      cout << "coils: ";
      for (int i = 0; i < ncoils; i++) {

        cout << coil[i];
      }
      cout << endl;
    }
    else {
      cerr << "Unable to read coils ! "  << mb.lastError() << endl;
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
