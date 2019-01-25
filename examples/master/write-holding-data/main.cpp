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
  string port ("/dev/ttyUSB2");

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

    mb.setSlave (33); // to the slave at address 33
    
    // reads and print previous values
    if (mb.readRegisters (1, registers, 4) > 0) {

      cout << "R0=" << registers[0].value() << endl;
      cout << "R1=" << registers[1].value() << endl;
      cout << "R2=" << registers[2].value() << endl;
      cout << "R3=" << registers[3].value() << endl;
    }
    else {
      cerr << "Unable to read input registers !"  << endl;
      exit (EXIT_FAILURE);
    }
    
    // modify to new values
    registers[0] = 152.3;
    registers[1] = 1010.7;
    registers[2] = 45;
    registers[3] = 901;
    // then writing to registers
    if (mb.writeRegisters (1, registers, 4) < 0) {

      cerr << "Unable to write input registers !"  << endl;
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
