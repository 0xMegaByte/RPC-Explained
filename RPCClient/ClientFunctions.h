/*
Copyright (C) 2023 Matan Shitrit (0xMegaByte)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
void PrintHelpRPCClient();