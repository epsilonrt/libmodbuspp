// Reads floating point holding registers from SolarPi pressure meter

// The pressure sensor calibration is composed of 4 decimal values.
// The first 2 are the minimum and maximum pressure calibration values in %RH.
// The following 2 are the output values of the analog-to-digital converter
// (in LSB) corresponding to the calibration values.

// This example code is in the public domain.
#include <iostream>
#include <string>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

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

  if (mb.open ()) { // open a connection
    // success, do what you want here

    // the bytes in the registers are arranged in big endian.
    // the solarpi calibration registers are arranged in little endian.
    Data<float, EndianBigLittle> registers[4];

    mb.setSlave (33); // for SolarPi Pressure board
    
    // reads values ....
    if (mb.readRegisters (1, registers, 4) > 0) {

      // then print them !
      cout << "R0=" << registers[0].value() << endl;
      cout << "R1=" << registers[1].value() << endl;
      cout << "R2=" << registers[2].value() << endl;
      cout << "R3=" << registers[3].value() << endl;
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
