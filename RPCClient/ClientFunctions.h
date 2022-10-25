#pragma once
#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string>
#include <stdarg.h>
#include <vector>
#include "..\IDL\RPC-Explained.h"

extern bool bImplicit;

void ClientDebugOutput(const char* format, ...);

#define CHECK( status ) if(status){ exit(status);}
#define CLIENT_OUT(str,...) ClientDebugOutput(str,__VA_ARGS__)

enum class ERpcServerCommandType
{
	SC_TERMINATE,
	SC_NAVIGATE,
	SC_STOP,
	SC_GET,
	SC_SET,
};
enum class ERpcArgumentType
{
	ARG_LOCATION,
	ARG_DESTINATION,
	ARG_DESTINATIONS,
	ARG_NONE
};

void UserInputLoop(handle_t hBinding = NULL);
void ParseUserInput(ERpcServerCommandType ECommand, ERpcArgumentType EArg,void* pArg, HANDLE hBinding);

template <typename Out>
void SerializeUserInput(const std::string& refUserInput, char delimiter, Out result);
std::vector<std::string> SerializeUserInput(const std::string& refUserInput, char delimiter);

void SetBindingType();
void PrintRPCClient();