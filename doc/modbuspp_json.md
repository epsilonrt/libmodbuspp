# JSON File Format for modbuspp

modbuspp uses the JSON (JavaScript Object Notation) format to describe Modbus masters, servers, and routers. This format is lightweight, easy to read and write, and widely used for data exchange. It is based on a simple syntax that allows representing objects and arrays.

These files allow configuring a modbuspp program, adapting it to the hardware environment and user needs without modifying the source code (and thus recompiling), or passing command-line parameters.

Here are the main elements of JSON syntax:

- Data is presented as key/value pairs separated by `:`
- Elements are separated by commas
- Curly braces `{}` denote objects
- Square brackets `[]` denote arrays

A JSON file as a whole is an anonymous object, thus enclosed in curly braces and containing objects. An object is preceded by a key, which is a string, followed by a colon `:` and the associated value. In modbuspp, objects are used to describe:

- masters, which are objects managed by the `Master` class and its slaves `Slave`,
- servers, which are objects managed by the `Server` class and its slaves `BufferedSlave`,
- routers, which are objects managed by the `Router` class, an extension of the `Server` class.

These three types of objects are `Device`s that share common properties.

Each object can contain properties that will be ignored by modbuspp but may be useful for the user. For example, you can add a `name` property to identify a master, server, or router. Since JSON does not support comments, you can use a property to add extra information. It is customary to start these properties with an underscore `_` to indicate that they are user-specific and not related to modbuspp. For example, you can add a `_comment` property for explanatory comments.

Note that to understand the structure of JSON objects, it is useful to refer to the modbuspp documentation, which describes the associated classes and functions. JSON objects are used to configure these classes and their instances.

You can read this document in its [French version](https://github.com/epsilonrt/libmodbuspp/blob/master/doc/modbuspp_json_fr.md)

## Device

A Device is a JSON object that contains information about a Modbus device; it describes a Modbus connection. Here is an example:

```json
{
  "example-device": {
    "mode": "rtu",
    "connection": "/dev/tnt0",
    "settings": "38400E1",
    "debug": true,
    "response-timeout": 500,
    "byte-timeout": 500,
    "rtu": {
      "mode": "rs232"
    },
    "_comment": "This is a simple but incomplete example of a Modbus RTU master."
  }
}
```
In this example, we have a JSON object describing a Modbus RTU device identified by the `example-device` key in the root object. It contains several fields describing the Modbus connection, as well as an `rtu` object for RTU-specific parameters.

These fields are used to describe the Modbus connection of a master (`Master` class), a server (`Server` class), or a router (`Router` class).

The `mode`, `connection`, and `settings` fields are mandatory:

- `mode`: Communication mode, can be `rtu` or `tcp`. Linked to the `Net` enumeration.
  - `rtu`: for a serial Modbus RTU connection.
  - `tcp`: for a TCP/IP Modbus TCP connection.
- `connection`: Serial connection path, IP address (v4 or v6), or hostname for TCP. For a TCP server, you can use `*` to listen on all interfaces. For a serial connection, it's usually a path like `/dev/ttyS1`, `/dev/ttyUSB0`, `COM1`, etc.
- `settings`: Serial connection parameters, e.g., `38400E1` for 38400 baud, 8 data bits, no parity, 1 stop bit. Port number for TCP.

The function related to these 3 fields is `Device::setBackend()`

Other fields are optional, here is their description:

- `debug`: If `true`, enables debug mode to display Modbus requests and responses. Related function: `Device::setDebug()`.
- `response-timeout`: Response timeout in milliseconds. Related function: `Device::setResponseTimeout()`.
- `byte-timeout`: Timeout for each received byte in milliseconds. Related function: `Device::setByteTimeout()`.
- `rtu`: Object for RTU-specific parameters.
  - `mode`: RS485 line mode, can be `rs485` or `rs232`. Related function: `Device::setSerialMode()`.
  - `rts`: RTS line state, can be `up`, `down`, or `none`, default is `none`. Related function: `Device::setRts()`.
  - `rts-delay`: RTS line delay in milliseconds. Related function: `Device::setRtsDelay()`.
- `recovery-link`: Enables automatic reconnection in case of link loss. Related function: `Device::setRecoveryLink()`.

## Master

A master is a Device that sends Modbus requests to one or more slaves of the `Slave` class; the program can thus perform all operations inherent to this class: read and write registers, inputs, coils, etc. Here is an example of a Modbus RTU master with several slaves:

```json
{
  "modbuspp-master": {
    "name": "rs485",
    "mode": "rtu",
    "connection": "/dev/ttyS1",
    "settings": "38400E1",
    "debug": true,
    "response-timeout": 500,
    "byte-timeout": 500,
    "rtu": {
      "mode": "rs485",
      "rts": "down"
    },
    "slaves": [
      {
        "id": 32
      },
      {
        "id": 33
      },
      {
        "id": 34,
        "pdu-adressing": true
      },
      {
        "id": 35
      }
    ]
  }
}
```

In this example, the master is configured to communicate with four slaves with IDs 32, 33, 34, and 35. The function related to these fields is `Master::addSlave()`.

Each object in the `slaves` array represents a Modbus slave identified by its `id` (between 1 and 247). You can add a `pdu-adressing` property to specify PDU addressing mode (data addressing starts at 0). Related function: `Master::setPduAddressing()`.

A master only needs to configure the Modbus connection and the list of slaves it communicates with. It only needs to know each slave's ID and optionally the PDU addressing mode. There is no configuration for data tables, as a master does not manage data; it simply reads or writes it in the slaves.

## Server

A server is a Device that receives Modbus requests from a master it is connected to. A server implements one or more slaves of the `BufferedSlave` class, which themselves implement the Modbus data tables: input registers (`input-register`), holding registers (`holding-register`), coils (`coil`), and discrete inputs (`discrete-input`).

The `Server` class associated with the `BufferedSlave` class allows implementing Modbus slaves in software, configurable via a JSON file.

Here is an example of a Modbus TCP server with one slave:

```json
{
  "modbuspp-server": {
    "mode": "tcp",
    "connection": "localhost",
    "settings": "1502",
    "debug": true,
    "recovery-link": true,
    "response-timeout": 500,
    "byte-timeout": 500,
    "slaves": [
      {
        "id": 10,
        "blocks": [
          {
            "table": "holding-register",
            "quantity": 4,
            "data-type": "float",
            "starting-address": 1,
            "endian" : "cdab",
            "values" : [1.5,-3.14,5.23e12,1.63e-6]
          },
          {
            "table": "input-register",
            "quantity": 2,
            "values" : [101,"0x100"]
          },
          {
            "table": "coil",
            "quantity": 12,
            "values" : ["0x5A",true,1,false,0]
          },          
          {
            "table": "discrete-input",
            "quantity": 4,
            "values" : [0,0,1,1]
          }
        ]
      }
    ]  
  }
}
```

The first part of the file repeats the fields described in Device. These parameters are followed by a `slaves` array containing the slaves implemented by the server. Here, we see only one with ID `10`. It is configured to implement four data tables: `holding-register`, `input-register`, `coil`, and `discrete-input`. These tables are described by objects in the `blocks` array, each having the following fields:

- `table`: The data table type, which can be `holding-register`, `input-register`, `coil`, or `discrete-input`. **This field is mandatory for all tables**.
- `quantity`: The number of elements in the table, i.e., the number of registers, inputs, coils, or discrete inputs. **This field is mandatory for all tables**.
- `starting-address`: The starting address for registers, only for `holding-register` and `input-register` tables. If not specified, the starting address is 1 (0 if the slave is in PDU mode).
- `data-type`: The data type for registers, can be `uint16`, `uint32`, `uint64`, `int16`, `int32`, `int64`, `float`, `double`, and `longdouble`. The `Data` model class manages these data types. Note that these types only store numeric values with a minimum size of 2 bytes. By default, the data type is `uint16`.
- `endian`: The endianness of data for registers, can be `abcd`, `cdab`, `badc`, and `dcba`. The default value is `abcd`. Related function: `Data::setEndianness()`.
- `values`: An array of initial values for the table. Values can be integers, floats for `holding-register` and `input-register` tables. For `coil` and `discrete-input` tables, values can be booleans (`true` or `false`), integers (`0` or `1`), or hexadecimal values (e.g., `0x5A`).

There must be **at least one element in the `blocks` array**, and each block must have at least the `table` and `quantity` fields. Other fields are optional.

The description of implemented slaves is much more complete than for a master, as a server can implement several data tables. The related function is `Server::addSlave()` to add a slave, and `BufferedSlave::addBlock()` to add a data table to a slave.

## Router

A router is a Device that allows implementing several masters connected to a server that waits for requests from "external" masters and routes the requests to the correct master based on the requested slave ID. It is thus possible to have a Modbus TCP or RTU router that communicates with several masters, which can be in RTU or TCP mode.

A router has at least 2 connections: an external connection on which the router listens for requests from masters (equivalent to the WAN port of TCP routers), and an internal connection on which it communicates as a master with one or more slaves (equivalent to the LAN port of TCP routers). You can add other internal connections.

To handle requests from outside and possibly indicate to the remote master that a register is not accessible, the router must know the mapping of each slave it manages. It must therefore know which slaves are connected to it and which registers, inputs, coils, and discrete inputs its slaves manage.

Here is an example of a Modbus router with three connections: a TCP connection to the outside, and two serial connections to the inside:

```json
{
  "modbuspp-router": {
    "mode": "tcp",
    "connection": "localhost",
    "settings": "1502",
    "recovery-link": true,
    "debug": true,
    "response-timeout": 500,
    "byte-timeout": 500,
    "masters": [
      {
        "name": "rs485",
        "mode": "rtu",
        "connection": "/dev/ttyS1",
        "settings": "38400E1",
        "debug": true,
        "response-timeout": 500,
        "byte-timeout": 500,
        "rtu": {
          "mode": "rs485",
          "rts": "down"
        },
        "slaves": [
          {
            "id": 33,
            "blocks": [
              {
                "table": "input-register",
                "quantity": 6
              },
              {
                "table": "holding-register",
                "quantity": 8
              }
            ]
          }
        ]
      },
      {
        "name": "virtual-clock",
        "mode": "rtu",
        "connection": "/dev/tnt0",
        "settings": "38400E1",
        "debug": true,
        "response-timeout": 3000,
        "byte-timeout": 500,
        "slaves": [
          {
            "id": 10,
            "blocks": [
              {
                "table": "input-register",
                "quantity": 8
              },
              {
                "table": "holding-register",
                "quantity": 2
              },
              {
                "table": "coil",
                "quantity": 1
              }
            ]
          }
        ]
      }
    ]
  }
}
```

The first part of the file repeats the fields described in Device and corresponds to the configuration of the external connection.
Then comes a `masters` array containing the internal connections; each object in the array represents a Modbus master identified by its `name` (string). **The `name` field is mandatory to identify each master**.

Each object in the `masters` array contains a description of the internal connection (syntax identical to that of a Device), as well as a `slaves` array containing the slaves connected to this master. Each slave is described by an object in the `slaves` array with a syntax identical to that of a server (not a master, as previously indicated, the server connected to the outside needs to know the complete mapping).
