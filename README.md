# RPC Explained

This project is a simple demonstration of remote procedure call (RPC) communication using a client-server architecture. In this example, we use a spaceship and its navigation system to illustrate how RPC can be used in a real-world application.

## What is...?

### [RPC (Remote Procedure Call)](https://learn.microsoft.com/en-us/windows/win32/rpc/rpc-start-page)
a powerful technology for creating distributed client/server programs.
You can use RPC in all client/server applications based on Windows operating systems.
It can also be used to create client and server programs for heterogeneous network environments that include such operating systems as Unix and Apple.

### [IDL (Interface Definitions Language)](https://docs.microsoft.com/en-us/windows/win32/midl/interface-definition-idl-file)
IDL is a descriptive language to describe interfaces being implemented by objects.
files contains the building blocks of our spaceship.\
It contains the spaceship's interfaces such as engine, flight status system and their functions.

### [ACF (Application Configuration File)](https://learn.microsoft.com/en-us/windows/win32/rpc/the-acf-file)
In the RPC context, ACF files are used to configure the endpoints of remote services or components that can be accessed by RPC clients. They contain information such as the network address of the server, port number, and the server's interface.

---

## COM vs. RPC
COM and RPC are technologies used in software development for distributed systems. COM is used to create reusable software components that can communicate with each other using the COM interface. RPC is used to invoke a procedure or function on a remote machine. While both can be used for distributed systems, COM is focused on component-based software development, and RPC is focused on remote procedure invocation.

---

## Installation
### Dependencies

* [Windows 10 SDK (10.0.22621.0 or above)](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
* [Visual Studio 2022](https://visualstudio.microsoft.com/vs/whatsnew/)

Clone this repository to your local machine

```git
git clone https://github.com/0xMegaByte/RPC-Explained
cd RPC-Explained
cd IDL
```

## IDL Setup
* Open ```RPC-Explained.idl``` in an editor of your choice.
* Using ```Developer Command Prompt for VS 2022.exe``` do the following:
    * For every UUID in the idl file, generate your own UUID using ```uuidgen``` tool to avoid UUID collision
    * Replace the old UUIDs with the newly generated UUIDs.
    * After all the UUIDs replaced, save and compile the IDL file with the ACF file
    ```
    midl.exe /acf RPC-Explained.acf RPC-Explained.idl
    ```
    * Upon successful compilation, a total of 3 files are generated:\
        * RPC-Explained.h - Contains the declarations of the interfaces, structures, and data types defined in the IDL file. 
        * RPC-Explained_c.c - Client stub file
        * RPC_Explained_s.c - Server stub file

## Compile The Solution

* Open ```RPC-Explained.sln``` in Visual Studio 2022
* Build the solution

NOTE: Please ensure that both projects have their C++ version set to ```ISO C++20 Standard (/std:c++20)``` and platform toolset to ```v143```.
 
 ---

## Starting Up The Engines

The spaceship in this project has a navigation system that requires a destination to be set before it can navigate.\
Users can control the spaceship using the client component of the project.\
The following commands are available:

### <u>Server Related Client Commands</u>
- `implicit`: Use implicit rpc communication method with the server
- `explicit`: Use excplicit rpc communication method with the server
- `terminate`: Terminates the connection with the server

### <u>Navigation System Client Commands</u>
- `set destination`: Set the spaceship's destination. This command requires a parameter specifying the destination.
- `get destinations`: Get a list of available destinations.
- `get location`: Get the current location of the spaceship.\
The default location of the spaceship is `"0.00, 0.00"`, and it can only change if a destination has been set and the `navigate` command has been issued.
- `navigate`: Move the spaceship to a random location. This command can only be used if a destination has been set.\
<u>Disclaimer: </u>There isn't an actual system for calculating a path to the selected destination. The spaceship location changes randomly.
- `stop`: Stop the spaceship. This command can be used at any time to halt the spaceship's 
movement.
- `help`: display help

---

## Examples

To begin, you must select either the explicit or implicit binding protocol.
![RPC Client](/Pictures/RPCClient.png)

During startup, the server has already registered both binding protocols.
![RPC Client](/Pictures/RPCServer.png)


---

## Contributing

If you would like to contribute to this project, please feel free to submit a pull request. I welcome any suggestions or improvements that you may have.

## Authors

This project was created by Matan Shitrit [@0xMegaByte](https://twitter.com/0xMegaByte).

## License

This project is licensed under the [GPL-3.0](https://opensource.org/license/gpl-3-0/). See the LICENSE file for more information.

## Acknowledgments

[Introduction to RPC - Part 1 by Anders Dalvander](https://www.codeproject.com/Articles/4837/Introduction-to-RPC-Part-1)\
[MSDN - Remote Procedure Call (RPC)](https://learn.microsoft.com/en-us/windows/win32/rpc/rpc-start-page)