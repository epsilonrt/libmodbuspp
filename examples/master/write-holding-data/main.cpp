// Writes floating point holding registers to SolarPi humidity meter

// The humidity sensor calibration is composed of 4 decimal values.
// The first 2 are the minimum and maximum humidity calibration values in %RH.
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
    int ret;

    // the bytes in the registers are arranged in big endian.
    // the solarpi calibration registers are arranged in little endian.
    Data<float, EndianBigLittle> registers[4];

    mb.setSlave (33); // to the slave at address 33

    // set values ...
    registers[0] = 152.3;
    registers[1] = 1010.7;
    registers[2] = 45;
    registers[3] = 901;
    
    // then writing to registers
    ret = mb.writeRegisters (1, registers, 4);
    
    if (ret < 0) {

      cerr << "Unable to write input registers ! "  << mb.lastError() << endl;
      exit (EXIT_FAILURE);
    }
    else {
      cout << ret << " registers written (16-bit)." << endl;
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
