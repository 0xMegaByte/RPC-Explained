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

#include "ClientFunctions.h"

bool bImplicit;

template <typename Out>
void SerializeUserInput(const std::string& refUserInput, char delimiter, Out result) {
	std::istringstream iss(refUserInput);
	std::string item;
	while (std::getline(iss, item, delimiter)) {
		*result++ = item;
	}
}

std::vector<std::string> SerializeUserInput(const std::string& refUserInput, char delimiter) {
	std::vector<std::string> tokens;
	SerializeUserInput(refUserInput, delimiter, std::back_inserter(tokens));
	return tokens;
}

void SetBindingType()
{
	bool bExitLoop = false;
	std::string BindingType("");
	CLIENT_OUT("Choose Binding protocol:\n\t[*]Implicit\n\t[*]Explicit\n");
	while (!bExitLoop)
	{
		CLIENT_OUT(">> ");
		std::getline(std::cin, BindingType);

		if (!BindingType.empty() && BindingType.size() >= 1)
		{
			const char* pcBindingType = BindingType.c_str();
			if (_strcmpi(pcBindingType, "implicit") == 0)
			{
				bExitLoop = bImplicit = true;
			}
			else if (_strcmpi(pcBindingType, "explicit") == 0)
				bExitLoop = true;
			else
				CLIENT_OUT("Invalid binding type\n");
		}
	}
}

void ParseUserInput(ERpcServerCommandType ECommand, ERpcArgumentType EArg, void* pArg, HANDLE hBinding)
{
	if (!bImplicit && !hBinding) //if using explicit and invalid binding handle
		return;

	RPC_STATUS rpcstatus = -1;
	std::string* Destination = nullptr;

	Coordinates coordiantes{};
	BSTR pwcsDestination = SysAllocString(L"");
	BSTR pwcsDestinations = SysAllocString(L"");

	switch (ECommand)
	{
	case ERpcServerCommandType::SC_TERMINATE:

		bImplicit ? Imp_TerminateNavigationServer() : Exp_TerminateNavigationServer(hBinding);
		CLIENT_OUT("Navigation server terminated!\n");
		break;
	case ERpcServerCommandType::SC_STOP:
		rpcstatus = bImplicit ? Imp_StopNavigation() : Exp_StopNavigation(hBinding);
		if (rpcstatus == RPC_S_OK)
			CLIENT_OUT("Spaceship navigation stopped\n");
		else if (rpcstatus == RPC_E_CALL_REJECTED)
			CLIENT_OUT("Spacship already stopped\n");
		else
			CLIENT_OUT("Failed to stop spaceship navigation\n");
		break;
	case ERpcServerCommandType::SC_NAVIGATE:
		rpcstatus = bImplicit ? Imp_NavigateSpaceship() : Exp_NavigateSpaceship(hBinding);
		if (rpcstatus == RPC_S_OK)
			CLIENT_OUT("Navigating spaceship\n");
		else if (rpcstatus == RPC_E_CALL_REJECTED)
			CLIENT_OUT("Spaceship already navigating\n");
		else
			CLIENT_OUT("Failed to navigate spaceship\n");
		break;
	case ERpcServerCommandType::SC_GET:
		switch (EArg)
		{
		case ERpcArgumentType::ARG_LOCATION:
			rpcstatus = bImplicit ? Imp_GetCurrentLocation(&coordiantes) : Exp_GetCurrentLocation(hBinding, &coordiantes);
			if (rpcstatus == RPC_S_OK)
				CLIENT_OUT("Spaceship's location: Lat. %lf | Lon. %lf\n", coordiantes.m_ldLatitude, coordiantes.m_ldLongitude);
			else
				CLIENT_OUT("Failed to fetch spaceship's location\n");
			break;
		case ERpcArgumentType::ARG_DESTINATION:
			if (pwcsDestination)
			{
				rpcstatus = bImplicit ? Imp_GetDestination(&pwcsDestination) : Exp_GetDestination(hBinding, &pwcsDestination);
				if (rpcstatus == RPC_S_OK && pwcsDestination)
					CLIENT_OUT("Spaceship destination: %ls\n", pwcsDestination);
				else
					CLIENT_OUT("Destination unavailable\n");

				if (pwcsDestination)
					SysFreeString(pwcsDestination); //Imp_GetDestination might return null
			}
			break;
		case ERpcArgumentType::ARG_DESTINATIONS:
			if (pwcsDestinations)
			{
				rpcstatus = bImplicit ? Imp_GetAvailableDestinations(&pwcsDestinations) : Exp_GetAvailableDestinations(hBinding, &pwcsDestinations);
				if (rpcstatus == RPC_S_OK && pwcsDestination)
					CLIENT_OUT("Available Destinations:\n%ls\n", pwcsDestinations);
				else
					CLIENT_OUT("Failed to fetch available destinations\n");

				if (pwcsDestinations)
					SysFreeString(pwcsDestinations);
			}
			break;
		default:
			break;
		}
		break;
	case ERpcServerCommandType::SC_SET:

		switch (EArg)
		{
		case ERpcArgumentType::ARG_DESTINATION:

			Destination = (std::string*)pArg;
			if (Destination->size())
			{
				std::wstring wsDestination(Destination->begin(), Destination->end());
				BSTR bstrDestination = SysAllocString(wsDestination.c_str());
				if (bstrDestination)
				{
					rpcstatus = bImplicit ? Imp_SetDestination(bstrDestination) : Exp_SetDestination(hBinding, bstrDestination);
					if (rpcstatus == RPC_S_OK)
						CLIENT_OUT("Destination set to %ls\n",bstrDestination);
					else if (rpcstatus == RPC_E_CALL_REJECTED)
						CLIENT_OUT("Ongoing navigation. Stop spaceship to change destination\n");
					else
						CLIENT_OUT("Failed to set destination\n");

					SysFreeString(bstrDestination);
				}
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	if (pwcsDestination)
		SysFreeString(pwcsDestination);
	if (pwcsDestinations)
		SysFreeString(pwcsDestinations);

}

void UserInputLoop(handle_t hBinding)
{
	bool bTerminate = false;
	std::vector<std::string> vInputTokens;
	std::string UserInput("");
	size_t nUserInputTokens = -1;


	while (!bTerminate)
	{
		CLIENT_OUT(">> ");
		std::getline(std::cin, UserInput);
		if (!UserInput.empty())
		{
			vInputTokens = SerializeUserInput(UserInput, ' ');
			nUserInputTokens = vInputTokens.size();
			switch (nUserInputTokens)
			{
			case 1:
			{
				const char* pcToken = vInputTokens[0].c_str();
				if (_strcmpi(pcToken, "terminate") == 0)
				{
					ParseUserInput(ERpcServerCommandType::SC_TERMINATE, ERpcArgumentType::ARG_NONE, nullptr, hBinding);
					bTerminate = true;
				}

				else if (_strcmpi(pcToken, "stop") == 0)
					ParseUserInput(ERpcServerCommandType::SC_STOP, ERpcArgumentType::ARG_NONE, nullptr, hBinding);

				else if (_strcmpi(pcToken, "navigate") == 0)
					ParseUserInput(ERpcServerCommandType::SC_NAVIGATE, ERpcArgumentType::ARG_NONE, nullptr, hBinding);

				else if (_strcmpi(pcToken, "?") == 0 || _strcmpi(pcToken, "help") == 0)
					PrintHelpRPCClient();

				else
					CLIENT_OUT("Invalid command\n");
				break;
			}
			case 2:
			{
				const char* pcToken = vInputTokens[0].c_str();
				if (_strcmpi(pcToken, "get") == 0)
				{
					pcToken = vInputTokens[1].c_str();
					if (_strcmpi(pcToken, "location") == 0)
						ParseUserInput(ERpcServerCommandType::SC_GET, ERpcArgumentType::ARG_LOCATION, nullptr, hBinding);

					else if (_strcmpi(pcToken, "destination") == 0)
						ParseUserInput(ERpcServerCommandType::SC_GET, ERpcArgumentType::ARG_DESTINATION, nullptr, hBinding);

					else if (_strcmpi(pcToken, "destinations") == 0)
						ParseUserInput(ERpcServerCommandType::SC_GET, ERpcArgumentType::ARG_DESTINATIONS, nullptr, hBinding);

					else
						CLIENT_OUT("Invalid command\n");
				}
				else
					CLIENT_OUT("Invalid command\n");
				break;
			}
			case 3:
			{
				const char* pcToken = vInputTokens[0].c_str();
				if (_strcmpi(pcToken, "set") == 0)
				{
					pcToken = vInputTokens[1].c_str();
					if (_strcmpi(pcToken, "destination") == 0)
						ParseUserInput(ERpcServerCommandType::SC_SET, ERpcArgumentType::ARG_DESTINATION, &vInputTokens[2], hBinding);
					else
						CLIENT_OUT("Invalid command\n");
				}
				else
					CLIENT_OUT("Invalid command\n");
				break;
			}
			default:
				break;
			}
		}

	}
}

void PrintRPCClient()
{
	std::wcout <<
		R"(
  _____  _____   _____ _____ _ _            _   
 |  __ \|  __ \ / ____/ ____| (_)          | |  
 | |__) | |__) | |   | |    | |_  ___ _ __ | |_ 
 |  _  /|  ___/| |   | |    | | |/ _ \ '_ \| __|
 | | \ \| |    | |___| |____| | |  __/ | | | |_ 
 |_|  \_\_|     \_____\_____|_|_|\___|_| |_|\__|
################################################
 Made by Matan Shitrit (0xMegaByte) 2022.
################################################
		)" << "\n";
}
void PrintHelpRPCClient()
{
	std::wcout <<
		R"(
COMMANDS
--------
terminate		- Terminate the connection to the server.
stop			- Stops the spaceship.
navigate		- Navigates the spaceship to the chosen destination.

get location		- Gets spaceship's coordinates
get destination		- Gets spaceship's destination
get destinations	- Gets available destinations

set destination		- Sets spaceship's destination (use 'get destinations' command to get available destinations)
		)" << "\n";
}

void ClientDebugOutput(const char* format, ...)
{
	printf("[client] ");
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
