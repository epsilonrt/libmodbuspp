# How to build this example ?

This example is intended to be compiled with [Codelite](https://codelite.org).

[Codelite](https://codelite.org) is a free cross-platform software...

If you want to compile it without codelite, you can do it with [GNU make](https://www.gnu.org/software/make/) :

		make -f clock-server.mk

You can then run the program that is in the Debug folder.

Nevertheless, I am not sure that it works on Windows™ and macOS ™, so you 
should follow my advice and use CodeLite...

This library provides a file for pkg-config that should also allow you to 
compile with gcc:

		g++ -o clock-server $(pkg-config --cflags --libs modbuspp) main.cpp

