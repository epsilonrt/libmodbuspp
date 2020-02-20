// Read/Writes floating point holding registers to SolarPi pressure meter
// the MODBUS master is configured from the JSON ./master.json file

// The pressure sensor calibration is composed of 4 decimal values.
// The first 2 are the minimum and maximum pressure calibration values in hPa.
// The following 2 are the output values of the analog-to-digital converter
// (in LSB) corresponding to the calibration values.

// This example code is in the public domain.
#include <iostream>
#include <string>
#include <modbuspp.h>
#include <modbuspp/popl.h>

using namespace std;
using namespace Modbus;

int main (int argc, char **argv) {
  string jsonfile ("../master.json");

  if (argc > 1) {

    jsonfile = argv[1]; // the JSON filename can be provided as a parameter on the command line.
  }

  Master mb (jsonfile, "modbuspp-master");
  Slave & slv = mb.slave (33); // SolarPi Pressure meter

  cout << "Read/Write holding registers of slave[" << slv.number() << "] on " <<
       mb.connection() << " (" << mb.settings() << ")" << endl;

  if (mb.open ()) { // open a connection
    // success, do what you want here
    int ret;

    Data<float, EndianBigLittle> backup[4];
    // the bytes in the registers are arranged in big endian.
    // the solarpi calibration registers are arranged in little endian.
    Data<float, EndianBigLittle> bank1[4];
    Data<float, EndianBigLittle> bank2[4];

    // reads values ....
    if (slv.readRegisters (1, backup, 4) > 0) {

      // then print them !
      cout << "R0=" << backup[0].value() << endl;
      cout << "R1=" << backup[1].value() << endl;
      cout << "R2=" << backup[2].value() << endl;
      cout << "R3=" << backup[3].value() << endl;
    }
    else {
      cerr << "Unable to read holding registers ! "  << mb.lastError() << endl;
      exit (EXIT_FAILURE);
    }

    // set values ...
    bank1[0] = 152.3;
    bank1[1] = 1010.7;
    bank1[2] = 45;
    bank1[3] = 901;

    // then writing to registers
    ret = slv.writeRegisters (1, bank1, 4);
    if (ret < 0) {

      cerr << "Unable to write holding registers ! "  << mb.lastError() << endl;
      exit (EXIT_FAILURE);
    }
    else {
      cout << ret << " registers written (16-bit)." << endl;
    }

    // check if the values have been written
    if (slv.readRegisters (1, bank2, 4) > 0) {
      bool ok = true;
      
      for (int i = 0; i < 4; i++) {
        ok = ok && (bank1[i] == bank2[i]);
      }
      
      if (ok) {

        cout << "Registers have been correctly written" << endl;
      }
      else {

        cout << "Registers were not correctly written" << endl;
        // then print them !
        cout << "R0=" << bank2[0].value() << endl;
        cout << "R1=" << bank2[1].value() << endl;
        cout << "R2=" << bank2[2].value() << endl;
        cout << "R3=" << bank2[3].value() << endl;
      }
    }
    else {
      cerr << "Unable to read holding registers ! "  << mb.lastError() << endl;
      exit (EXIT_FAILURE);
    }

    ret = slv.writeRegisters (1, backup, 4);
    if (ret < 0) {

      cerr << "Unable to write holding registers ! "  << mb.lastError() << endl;
      exit (EXIT_FAILURE);
    }
    else {
      cout << ret << " restored registers (16-bit)." << endl;
    }

    mb.close();
  }
  else {
    cerr << "Unable to open MODBUS connection to " << mb.connection() << " : "
         << mb.lastError() << endl;
    exit (EXIT_FAILURE);
  }

  return 0;
}
/* ========================================================================== */
