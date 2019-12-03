# libmodbuspp

_A C++ wrapper for the [libmodbus](https://libmodbus.org/) library_

## Abstract

The libmodbuspp library provides a C++ overlay to [libmodbus](https://libmodbus.org/), 
a wrapper, having no other dependency than libmodbus and libstdc++. 
libmodbuspp is a free software library to send/receive data according to the MODBUS protocol. 
This library supports RTU (serial) and TCP (Ethernet) communications.
libmodbussp allows you to implement MODBUS servers and clients.
libmodbussp will be used in version 2 of the utility [mbpoll](https://github.com/epsilonrt/mbpoll).
Like libmodbus, libmodbuspp is cross-platform and can be compiled on
Gnu/Linux and other Unix, Windows and MacOs.

The [Modbus::Master](http://www.epsilonrt.fr/modbuspp/classModbus_1_1Master.html)
class allows you to design clients (also called masters), who communicate with 
servers (also called slaves).
That is, Modbus::Master can read or write data to slaves.
To do this, Modbus::Master is associated with the 
[Modbus::Slave](http://www.epsilonrt.fr/modbuspp/classModbus_1_1Slave.html) 
class which allows to perform the read and write operations, 
here are an example of use:

```cpp
uint16_t values[2]; // array to store the values of the input registers
Master mb (Rtu, "COM1", "19200E1"); // new master on RTU
mb.open(); // open a connection
Slave & slv = mb.addSlave (33); // to the slave at address 33
slv.readInputRegisters (1, values, 2);
cout << values[0] << endl;
cout << values[1] << endl;
```

This example reads the input registers number 1 and 2 of the MODBUS slave 
(server) at address 33. The source code of this program is searchable
in the examples folder 
[read-input-registers](https://github.com/epsilonrt/libmodbuspp/blob/master/examples/master/read-input-registers/main.cpp)

The Modbus::Server class is used to design servers. An object of the class
Modbus::Server allows to implement one or **more** slaves that can be
dissociated by the use of a different address.
The Modbus::Server class is associated with the 
[Modbus::BufferedSlave](http://www.epsilonrt.fr/modbuspp/classModbus_1_1BufferedSlave.html) 
class to implement slaves.
Modbus::BufferedSlave used by Modbus::Server is derived from Modbus::Slave
used by Modbus::Master, it allows to define data blocks corresponding to each 
MODBUS entity: input registers, discrete inputs, holding registers and coils. 

Here is an example of use:

```cpp
// ModBus Data
bool daylight;// daylight saving time, true = summer time
Data<int32_t> gmtoff; // GMT offset, +/- minutes
uint16_t mb_time[8]; // date and time
Server srv (Tcp, "127.0.0.1", "1502"); // new Modbus Server on TCP/IP
BufferedSlave & slv = srv.addSlave (10); // Adding a new slave to the server
// defines the data blocks of our slave slv
slv.setBlock (InputRegister, 8); // date and time
slv.setBlock (HoldingRegister, 2); // GMT offset, +/- minutes
slv.setBlock (Coil, 1); // daylight saving time, ON = summer time
//...
slv.writeCoil (1, daylight);
slv.writeRegister (1, gmtoff);
if (srv.open ()) { // open a connection
  for (;;) {
    now = time (nullptr);
    if (now > before) {
      before = now;
      // update daylight saving time from holding register
      slv.readCoil (1, daylight);
      // update GMT offset from holding register
      slv.readRegister (1, gmtoff);
      // calculate the epoch time
      now += (daylight ? 3600 : 0) + gmtoff;
      t = gmtime (&now);
      mb_time[0] = t->tm_sec;
      mb_time[1] = t->tm_min;
      mb_time[2] = t->tm_hour;
      mb_time[3] = t->tm_mday;
      mb_time[4] = t->tm_mon + 1;
      mb_time[5] = t->tm_year + 1900;
      mb_time[6] = t->tm_wday;
      mb_time[7] = t->tm_yday + 1;
      // update the input registers
      slv.writeInputRegisters (1, mb_time, 8);
    }
    srv.poll (100);
  }
}
```

This example realizes a MODBUS time server which has the slave address 10.
The source code of this program is searchable in the examples folder
[clock-server](https://github.com/epsilonrt/libmodbuspp/blob/slave/examples/server/clock-server/main.cpp).

As can be seen in this example libmodbuspp also has a template class
[Modbus::Data](http://www.epsilonrt.fr/modbuspp/classModbus_1_1Data.html)
that allows to easily manage registers of a size greater than or 
equal to 16 bits. This class allows you to manipulate all arithmetic types int, 
long, float, double... in the form of MODBUS registers in a transparent manner 
for the user.
The default storage order of bytes and words is the big Indian but
can be chosen when declaring the variable using enumeration
[Modbus::Endian](http://www.epsilonrt.fr/modbuspp/namespaceModbus.html#a64c21ecb024ca9647d853c9439f850d2), 
for example:

```cpp
Data<float,EndianBigLittle> voltage;
```

allows to define a floating number (32-bit) with a byte order in big Indian 
and a word organization in little Indian (CDAB).

A Modbus::BufferedSlave object is also able to relay the requests it receives to:

* "real" slaves connected by RTU (or TCP) to the machine running
the server (Modbus::Server).
* callback functions that the user of the library can define in order to
to perform a custom processing of requests.

This gives the objects in the Modbus::Server class routing capabilities.
Here is an example of a server that illustrates this routing capability:

```cpp
// Creating the MODBUS master link that controls the serial link
Master mb (Rtu, "/dev/ttyUSB0", "38400E1"); // new master on RTU
if (mb.open()) { // Opening the master link if successful, create the server
  // New TCP server listens on host and port
  Server srv (Tcp, "127.0.0.1", "1502");
  // Declaration of a slave with the address 33 accessible via the TCP server
  // and physically connected to the serial link managed by mb
  BufferedSlave & press = srv.addSlave (33, &mb);
  // Declaration of the input register block of the press slave
  press.setBlock (InputRegister, 6);
  // Declaration of the block of holding registers of the press slave,
  press.setBlock (HoldingRegister, 8); // 32-bit * 4
  // Message and a pointer to the object of the class Device that manages the
  // master link (& mb in this example)
  press.setBeforeReplyCallback (beforeReplyCB);
  press.setAfterReplyCallback (afterReplyCB);
  if (srv.open ()) {
    // if the startup is successful, we perform a polling loop that performs
    // all server operations:
    // - receive requests on TCP
    // - redirection of requests to the serial link
    // - recovery of information coming from the serial link and storage in a memory buffer
    // - response to the TCP client with its information
    for (;;) {
      srv.poll (1000);
    }
    srv.close();
  }
}
```

It is a gateway relaying requests received by TCP/IP to a slave (address 33) 
connected in RTU.
The source code of this program is searchable in the examples folder
[tcp-gateway](https://github.com/epsilonrt/libmodbuspp/blob/master/examples/server/tcp-gateway/main.cpp).

libmodbuspp comes with full documentation in manual and [html](http://www.epsilonrt.fr/modbuspp/) format.

## Quickstart guide

### Installation

The fastest and safest way to install libmodbuspp on Debian, Armbian, Raspbian ... 
is to use the APT repository from [piduino.org](http://apt.piduino.org), 
so you should do the following :

    wget -O- http://www.piduino.org/piduino-key.asc | sudo apt-key add -
    sudo add-apt-repository 'deb http://apt.piduino.org stretch piduino'
    sudo apt update
    sudo apt install libmodbuspp-dev libmodbuspp-doc 

This repository provides Piduino packages for `i386`, `amd64`, `armhf` and 
`arm64` architectures.  
In the above commands, the repository is a Debian Stretch distribution, but you 
can also choose Ubuntu Xenial or Bionic by replacing `stretch` with `xenial` or 
`bionic`. It may be necessary to install the `software-properties-common` 
package for `add-apt-repository`.

For Raspbian you have to do a little different :

    wget -O- http://www.piduino.org/piduino-key.asc | sudo apt-key add -
    echo 'deb http://raspbian.piduino.org stretch piduino' | sudo tee /etc/apt/sources.list.d/piduino.list
    sudo apt update
    sudo apt install libmodbuspp-dev libmodbuspp-doc

The Raspbian repository provides Piduino packages for `armhf` architecture for Stretch only.

If you want to build from sources, you can follow the 
[Wiki](https://github.com/epsilonrt/libmodbuspp/wiki/Build-from-source).

### How To build an example ?

Here is a complete example that can be compiled without error:

```cpp
#include <iostream>
#include <string>
#include <modbuspp.h>

using namespace std;
using namespace Modbus;

int main (int argc, char **argv) {
  string port ("/dev/ttyUSB0");

  Master mb (Rtu, port , "38400E1"); // new master on RTU

  if (mb.open ()) { // open a connection
    // success, do what you want here
    uint16_t values[2];

    Slave & slv = mb.addSlave (33); // to the slave at address 33
    if (slv.readInputRegisters (1, values, 2) == 2) {

      cout << "R0=" << values[0] << endl;
      cout << "R1=" << values[1] << endl;
    }
    else {
      cerr << "Unable to read input registers !"  << endl;
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
```

Enter the text of this program with your favorite text editor and save the file 
in `main.cpp`

To build, you must type the command:

    g++ -o read-input-registers main.cpp $(pkg-config --cflags --libs libmodbuspp)
    
You can then run it :

    ./read-input-registers 
    R0=9964
    R1=10029

With [Codelite](https://codelite.org/) it's easier and funny, right ? 

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/piduino/master/doc/images/codelite-2.png)

You will find several examples in the folder 
[/usr/share/doc/modbuspp/examples](https://github.com/epsilonrt/libmodbuspp/tree/master/examples)

## Documentation

The libmodbuspp-doc package provides documentation.

The classes provided by the library are documented by man pages:

* The **Modbus_Master** page for the `Modbus::Master` class  
* The **Modbus_Slave** page for the `Modbus::Slave` class  
* The **Modbus_Server** page for the `Modbus::Server` class  
* The **Modbus_BufferedSlave** page for the `Modbus::BufferedSlave` class  
* The **Modbus_Data** page for the `Modbus::Data` class  
* The **Modbus_RtuLayer** page for the `Modbus::RtuLayer` class  
* The **Modbus_TcpLayer** page for the `Modbus::TcpLayer` class  
* The **Modbus_Timeout** page for the `Modbus::Timeout` class  

The complete API is documented in the folder `/usr/share/doc/modbuspp/api-manual`

## About Modbus

MODBUS is considered an application layer messaging protocol, providing 
Master/Slave communication between devices connected together through buses or 
networks. 
On the OSI model, MODBUS is positioned at level 7. MODBUS is intended to be a 
request/reply protocol and delivers services specified by function codes. 
The function codes of MODBUS are elements of MODBUS’ request/reply PDUs 
(Protocol Data Unit).

In order to build the MODBUS application data unit, the client must initiate a 
MODBUS transaction. It is the function which informs the server as to which type 
of action to perform. The format of a request initiated by a Master is 
established by the MODBUS application protocol. The function code field is then 
coded into one byte. Only codes within the range of 1 through 255 are 
considered valid, with 128-255 being reserved for exception responses. 
When the Master sends a message to the Slave, it is the function code field 
which informs the server of what type of action to perform.

To define multiple actions, some functions will have sub-function codes added to 
them. For instance, the Master is able to read the ON/OFF states of a group of 
discreet outputs or inputs. 
It could also read/write the data contents of a group of MODBUS registers. 
When the Master receives the Slave response, the function code field is used by 
the Slave to indicate either an error-free response or an exception response. 
The Slave echoes to the request of the initial function code in the case of a 
normal response.
