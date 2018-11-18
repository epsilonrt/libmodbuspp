# PiDuino

_Arduino on Pi boards, the best of both worlds !_

## Abstract

PiDuino is a C ++ library for Pi boards that allows the use of I/O like GPIO,
I2C, SPI, UART ... with an API as close as possible to the Arduino language.  
The description of Pi cards uses a stored "Object" model in a database that 
allows to add new models of boards easily.  

At this time, the SoC models supported are AllWinner H-Series and Broadcom 
BCM2708 through 2710 which allows it to be used on Raspberry Pi and most Nano Pi, 
Orange Pi and Banana Pi.

To learn more about PiDuino, you can follow the 
[Wiki](https://github.com/epsilonrt/libmodbuspp/wiki), but if you're in a hurry, 
let's go to the quick start version...

## Quickstart guide

The fastest and safest way to install modbuspp on Armbian is to use the APT 
repository from [modbuspp.org](http://apt.modbuspp.org), so you should do the following :

    wget -O- http://www.modbuspp.org/modbuspp-key.asc | sudo apt-key add -
    sudo add-apt-repository 'deb http://apt.modbuspp.org stretch modbuspp'
    sudo apt update
    sudo apt install libmodbuspp-dev modbuspp-utils

This repository provides libmodbuspp packages for `armhf` and `arm64` architectures.
In the above commands, the repository is a Debian Stretch distribution, but you 
can also choose Ubuntu Xenial or Bionic by replacing `stretch` with `xenial` or 
`bionic`. It may be necessary to install the `software-properties-common` 
package for `add-apt-repository`.

For Raspbian you have to do a little different :

    wget -O- http://www.modbuspp.org/modbuspp-key.asc | sudo apt-key add -
    echo 'deb http://raspbian.modbuspp.org stretch modbuspp' | sudo tee /etc/apt/sources.list.d/modbuspp.list
    sudo apt update
    sudo apt install libmodbuspp-dev modbuspp-utils

The Raspbian repository provides libmodbuspp packages for `armhf` architecture for Stretch only.

If you want to build from sources, you can follow the 
[Wiki](https://github.com/epsilonrt/libmodbuspp/wiki/Build-from-source).
    
### Utilities

Once installed, you should run the following on the command line :

    $ pinfo
    Name            : NanoPi Core2 Mini Shield
    Family          : NanoPi
    Database Id     : 40
    Manufacturer    : Friendly ARM
    Board Tag       : nanopineocore2shield
    SoC             : H5 (Allwinner)
    Memory          : 1024MB
    GPIO Id         : 9
    I2C Buses       : /dev/i2c-0
    SPI Buses       : /dev/spidev1.0
    Serial Ports    : /dev/ttyS1

As we can imagine, in the example, we are on a 
[NanoPi Neo Core2](http://wiki.friendlyarm.com/wiki/index.php/NanoPi_NEO_Core2) 
connected to a [Mini Shield](http://wiki.friendlyarm.com/wiki/index.php/Mini_Shield_for_NanoPi_NEO_Core/Core2).

To read the pin status of connector 1, run the following on the command line :

    $ pido readall 1
                                              CON1 (#1)
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | Pull | V | Ph || Ph | V | Pull | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    |     |     |     3.3V |      |      |   |  1 || 2  |   |      |      | 5V       |     |     |
    |  12 |   8 |  I2C0SDA | ALT2 |  OFF |   |  3 || 4  |   |      |      | 5V       |     |     |
    |  11 |   9 |  I2C0SCK | ALT2 |  OFF |   |  5 || 6  |   |      |      | GND      |     |     |
    |  91 |   7 |  GPIOG11 |  OFF |  OFF |   |  7 || 8  |   | OFF  | ALT2 | UART1TX  | 15  | 86  |
    |     |     |      GND |      |      |   |  9 || 10 |   | OFF  | ALT2 | UART1RX  | 16  | 87  |
    |   0 |   0 |   GPIOA0 |  OFF |  OFF |   | 11 || 12 |   | OFF  | OFF  | GPIOA6   | 1   | 6   |
    |   2 |   2 |   GPIOA2 |  OFF |  OFF |   | 13 || 14 |   |      |      | GND      |     |     |
    |   3 |   3 |   GPIOA3 |  OFF |  OFF |   | 15 || 16 |   | OFF  | ALT2 | UART1RTS | 4   | 88  |
    |     |     |     3.3V |      |      |   | 17 || 18 |   | OFF  | ALT2 | UART1CTS | 5   | 89  |
    |  15 |  28 | SPI1MOSI | ALT2 |  OFF |   | 19 || 20 |   |      |      | GND      |     |     |
    |  16 |  24 | SPI1MISO | ALT2 |  OFF |   | 21 || 22 |   | OFF  | OFF  | GPIOA1   | 6   | 1   |
    |  14 |  29 |  SPI1CLK | ALT2 |  OFF |   | 23 || 24 |   | OFF  | ALT2 | SPI1CS   | 27  | 13  |
    |     |     |      GND |      |      |   | 25 || 26 |   | OFF  | OFF  | GPIOA17  | 11  | 17  |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | Pull | V | Ph || Ph | V | Pull | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+

`pido` and `pinfo` come with manpages...

### Blink Example

Arduino programming on Pi board ? We are going there !

```c++
#include <libmodbuspp.h> // all the magic is here ;-)

const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi

void setup() {
  // initialize digital pin ledPin as an output.
  pinMode (ledPin, OUTPUT);
}

void loop () {
  // Press Ctrl+C to abort ...
  digitalWrite (ledPin, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay (1000);                 // wait for a second
  digitalWrite (ledPin, LOW);   // turn the LED off by making the voltage LOW
  delay (1000);                 // wait for a second
}
```

Obviously, you need to know the pin number where you connected the LED ! 

    $ pido readall 1
                                              CON1 (#1)
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | Pull | V | Ph || Ph | V | Pull | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    |     |     |     3.3V |      |      |   |  1 || 2  |   |      |      | 5V       |     |     |
    |  12 |   8 |  I2C0SDA | ALT2 |  OFF |   |  3 || 4  |   |      |      | 5V       |     |     |
    |  11 |   9 |  I2C0SCK | ALT2 |  OFF |   |  5 || 6  |   |      |      | GND      |     |     |
    |  91 |   7 |  GPIOG11 |  OFF |  OFF |   |  7 || 8  |   | OFF  | ALT2 | UART1TX  | 15  | 86  |
    |     |     |      GND |      |      |   |  9 || 10 |   | OFF  | ALT2 | UART1RX  | 16  | 87  |
    |   0 |   0 |   GPIOA0 |  OFF |  OFF |   | 11 || 12 |   | OFF  | OFF  | GPIOA6   | 1   | 6   |
    |   2 |   2 |   GPIOA2 |  OFF |  OFF |   | 13 || 14 |   |      |      | GND      |     |     |
    |   3 |   3 |   GPIOA3 |  OFF |  OFF |   | 15 || 16 |   | OFF  | OFF  | GPIOG8   | 4   | 88  |
    |     |     |     3.3V |      |      |   | 17 || 18 |   | OFF  | OFF  | GPIOG9   | 5   | 89  |
    |  22 |  12 |   GPIOC0 |  OFF |  OFF |   | 19 || 20 |   |      |      | GND      |     |     |
    |  23 |  13 |   GPIOC1 |  OFF |  OFF |   | 21 || 22 |   | OFF  | OFF  | GPIOA1   | 6   | 1   |
    |  24 |  14 |   GPIOC2 |  OFF |  OFF |   | 23 || 24 |   | UP   | OFF  | GPIOC3   | 10  | 25  |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | Pull | V | Ph || Ph | V | Pull | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+


The iNo column corresponds to the 'Arduino' number, the number 0 pin corresponds
therefore at pin 11 of the GPIO connector (GPIOA0).

To build, you must type the command:

    $ g++ -o blink blink.cpp $(pkg-config --cflags --libs modbuspp)
    
You can then execute the program :

    $ sudo ./blink

`sudo` is necessary for an access to the memory mapping of the GPIO). 
You can enable the **setuid** bit to avoid `sudo` in the future :

    $ sudo chmod u+s blink
    $ ./blink

With [Codelite](https://codelite.org/) it's easier and funny, right ? 

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/libmodbuspp/master/doc/images/codelite-2.png)

You should read the [wiki on the examples](https://github.com/epsilonrt/libmodbuspp/wiki/Examples) to learn more...
