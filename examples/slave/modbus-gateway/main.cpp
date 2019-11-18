// libmodbuspp template
// This example code is in the public domain.
#include <iostream>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

int beforeReplyCB (Message & msg, Device * dev);
int afterReplyCB (Message & msg, Device * dev);

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  string host ("127.0.0.1");
  string port = "1502";
  string serial ("/dev/ttyUSB0");

  if (argc > 1) {

    serial = argv[1]; // the serial port can be provided as a parameter on the command line.
  }

  Master mb (Rtu, serial , "38400E1"); // new master on RTU
  // if you have to handle the DE signal of the line driver with RTS,
  // you should uncomment the lines below...
  // mb.rtu().setRts(RtsDown);
  // mb.rtu().setSerialMode(Rs485);
  mb.setDebug();

  if (mb.open()) {
    Server srv (Tcp, host, port); // new slave on TCP

    srv.setDebug();
    srv.setRecoveryLink();

    BufferedSlave & slv = srv.addSlave (33, &mb);  // SolarPi Pressure meter
    /* Input Registers
    @ Reg.  Size    Sensor  Description
    1       16-bit  MPX     Pression mesurée en dixièmes de hPa, entier non signé
    2       16-bit  HSC     Pression mesurée en dixièmes de hPa, entier non signé
    3       16-bit  HSC     Température mesurée en centièmes de °C, entier signé
    4       16-bit  MPX     Valeur brute ADC de la pression en LSB, entier non signé
    5       16-bit  HSC     Valeur brute HSC de la pression en LSB, entier non signé
    6       16-bit  HSC     Valeur brute HSC de la température en LSB, entier non signé
    */
    slv.setBlock (InputRegister, 6);
    /*
    @ Reg.  Size    Sensor  Description                                       Default
    1       32-bit  MPX     Pression minimale étalon, float en hPa            150
    3       32-bit  MPX     Pression maximale étalon, float en hPa            1150
    5       32-bit  MPX     Valeur ADC minimale étalon, float en quantum ADC  40.96
    7       32-bit  MPX     Valeur ADC maximale étalon, float en quantum ADC  962.56
     */
    slv.setBlock (HoldingRegister, 8); // 32-bit * 4

    slv.setBeforeReplyCallback (beforeReplyCB);
    slv.setAfterReplyCallback (afterReplyCB);

    cout << "Modbus TCP Gateway" << endl << "listening on " <<
         srv.backend().connection() << ":" << srv.backend().settings() << endl;

    if (srv.open ()) { // open a connection
      // success, do what you want here
      for (;;) {
        srv.task();
      }
      srv.close();
    }

  }

  return 0;
}

// -----------------------------------------------------------------------------
int beforeReplyCB (Message & msg, Device * dev) {
  cout << "<hello>" << endl;
  return 0;
}

// -----------------------------------------------------------------------------
int afterReplyCB (Message & msg, Device * dev) {
  cout << "<bye>" << endl;
  return 0;
}


/* ========================================================================== */
