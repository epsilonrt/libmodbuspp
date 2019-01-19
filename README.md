# libmodbuspp

_A C++ wrapper for the [libmodbus](https://libmodbus.org/) library_

## Abstract

libmodbus is a free software library to send/receive data according to the 
Modbus protocol. This library is written in C and supports RTU (serial) and 
TCP (Ethernet) communications.

The libmodbuspp library provides a C++ overlay to [libmodbus](https://libmodbus.org/), a wrapper,
having no other dependency than libmodbus and libstdc++

A good example always better than a long explanation, this is an extremely 
simple example:

```c++
uint16_t values[2]; // array to store the values of the input registers
Master mb (Rtu, port, "19200E1"); // new master on RTU
mb.open(); // open a connection
mb.setSlave (33); // to the slave at address 33
mb.readInputRegisters (1, values, 2);
cout << values[0] << endl;
cout << values[1] << endl;
```
This example reads the input registers number 1 and 2 of the MODBUS slave 
(server) at address 33.

## Quickstart guide

### Installation

The fastest and safest way to install libmodbuspp on Debian, Armbian, Raspbian ... 
is to use the APT repository from [piduino.org](http://apt.piduino.org), 
so you should do the following :

    wget -O- http://www.piduino.org/piduino-key.asc | sudo apt-key add -
    sudo add-apt-repository 'deb http://apt.piduino.org stretch piduino'
    sudo apt update
    sudo apt install libmodbuspp-dev 

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
    sudo apt install libmodbuspp-dev

The Raspbian repository provides Piduino packages for `armhf` architecture for Stretch only.

If you want to build from sources, you can follow the 
[Wiki](https://github.com/epsilonrt/libmodbuspp/wiki/Build-from-source).

### Example

Here is a complete example that can be compiled without error:

```c++
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

    mb.setSlave (33); // to the slave at address 33
    if (mb.readInputRegisters (1, values, 2) == 2) {

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

    $ g++ -o read-input-registers main.cpp $(pkg-config --cflags --libs libmodbuspp)
    
You can then run it :

    $ ./read-input-registers 
    R0=9964
    R1=10029

With [Codelite](https://codelite.org/) it's easier and funny, right ? 

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/piduino/master/doc/images/codelite-2.png)

You will find several examples in the folder `/usr/share/doc/modbuspp/examples` (or `/usr/local/share/...`) 
