#include "ServerFunctions.h"

RPCServer* g_pRPCServer = nullptr;
bool g_bTerminateLocationThread = false;

RPCServer::RPCServer() :
	m_pwcsDestination(nullptr), m_Coordinates(),
	m_hLocationThread(INVALID_HANDLE_VALUE),
	m_hLocationEvent(INVALID_HANDLE_VALUE),
	m_hStopEvent(INVALID_HANDLE_VALUE)
{
	m_AvailableDestinations =
	{
		{L"Earth"		,{937.88349, 412.47343}},
		{L"Asgard"		,{922.81677, 548.36541}},
		{L"Knowhere"	,{754.46421, 313.19910}},
		{L"Xandar"		,{660.32702, 681.08270}},
		{L"Vormir"		,{481.28034, 332.20725}}
	};

	m_Coordinates.m_ldLatitude = m_Coordinates.m_ldLongitude = 0;
}

RPCServer::~RPCServer()
{
	if (this->m_hLocationEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->m_hLocationEvent);
		this->m_hLocationEvent = INVALID_HANDLE_VALUE;
	}

	if (this->m_hLocationThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->m_hLocationThread);
		this->m_hLocationThread = INVALID_HANDLE_VALUE;
	}

	if (this->m_hStopEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->m_hStopEvent);
		this->m_hStopEvent = INVALID_HANDLE_VALUE;
	}

	if (this->m_pwcsDestination)
	{
		SysFreeString(this->m_pwcsDestination);
	}
}

void RPCServer::SetDestination(BSTR pwcsDestination)
{
	if (this->m_pwcsDestination)
	{
		SysFreeString(this->m_pwcsDestination);
		this->m_pwcsDestination = nullptr;
	}

	this->m_pwcsDestination = pwcsDestination;
}

DWORD WINAPI LocationThread(LPVOID lpv)
{
	RPCServer* pServer = (RPCServer*)lpv;
	unsigned int val = 0;
	if (pServer)
	{
		srand(42069);

		while (!g_bTerminateLocationThread)
		{
			//Waiting to LocationEvent to signaled (triggered) and wait when switch to non-signaled
			if (WaitForSingleObject(pServer->m_hLocationEvent, INFINITE) == WAIT_OBJECT_0)
			{
				val = rand();
				pServer->m_Coordinates.m_ldLatitude = (double)(val / 2);

				val = rand();
				pServer->m_Coordinates.m_ldLongitude = (double)(val / 2);

				SERVER_OUT("%s : latitude %lf | longitude %lf\n", __FUNCTION__
					, pServer->m_Coordinates.m_ldLatitude, pServer->m_Coordinates.m_ldLongitude);
			}
		}
	}
	return 0;
}

bool IsEventStateSignaled(HANDLE hEvent)
{
	return WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0 ? true : false;
}


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//=+		   				  IDL Functions Implementations							 =+
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

//------------------------------Implicit binding functions-----------------------------
void Imp_InitializeNavigationServer()
{
	if (!g_pRPCServer)
	{
		g_pRPCServer = new RPCServer();
		if (g_pRPCServer)
		{
			if (g_pRPCServer->m_hStopEvent == INVALID_HANDLE_VALUE)
				g_pRPCServer->m_hStopEvent = CreateEventW(NULL, TRUE, TRUE, NULL); //Initial state: signaled

			if (g_pRPCServer->m_hLocationEvent == INVALID_HANDLE_VALUE)
				//Creates Location monitor non-signaled state event to trigger in LocationThread
				g_pRPCServer->m_hLocationEvent = CreateEventW(NULL, TRUE, FALSE, NULL); //Initial state: non-signaled

			if (g_pRPCServer->m_hLocationThread == INVALID_HANDLE_VALUE)
				//Create location thread
				g_pRPCServer->m_hLocationThread = CreateThread(0, 0, LocationThread, g_pRPCServer, 0, 0);
		}
	}
}
void Imp_TerminateNavigationServer()
{
	SERVER_OUT("Closing navigation server via implicit shutdown..\n");

	RPC_STATUS rpcstatus = RpcMgmtStopServerListening(NULL);

	if (rpcstatus == RPC_S_OK)
	{
		if (g_pRPCServer)
		{
			delete g_pRPCServer;
			g_pRPCServer = nullptr;
		}
		SERVER_OUT("Navigation server terminated!\n");
	}
	else
	{
		SERVER_OUT("Failed to stop server! %ld\n", rpcstatus);
	}
}

RPC_STATUS Imp_GetCurrentLocation(Coordinates* pLocation)
{
	if (!g_pRPCServer)
		return RPC_E_FAULT;

	*pLocation = g_pRPCServer->m_Coordinates;

	return RPC_S_OK;
}
RPC_STATUS Imp_SetDestination(BSTR pwcsDestination)
{
	if (!g_pRPCServer || !pwcsDestination)
		return RPC_E_FAULT;

	if (g_pRPCServer->m_hLocationEvent == INVALID_HANDLE_VALUE)
		return RPC_E_FAULT;

	bool bSignaled = false;
	bool bFound = false;

	//Check event state
	if (IsEventStateSignaled(g_pRPCServer->m_hLocationEvent))
	{
		SERVER_OUT("On going navigation\n");
		return RPC_E_CALL_REJECTED;
	}


	AvailableDestinations& DestinationList = g_pRPCServer->m_AvailableDestinations;

	for (auto& Destination : DestinationList)
	{
		if (_wcsicmp(Destination.first, pwcsDestination) == 0)
		{
			g_pRPCServer->SetDestination(pwcsDestination);
			SERVER_OUT("Destination set to %ls\n", pwcsDestination);
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		SERVER_OUT("%ls not an available destination!\n", pwcsDestination);
		return RPC_E_FAULT;
	}

	return RPC_S_OK;
}
RPC_STATUS Imp_GetDestination(BSTR* ppwcsDestination)
{
	if (!g_pRPCServer || !ppwcsDestination)
		return RPC_E_FAULT;

	if (!g_pRPCServer->m_pwcsDestination)
		return RPC_E_FAULT;

	if (BSTR destination = SysAllocString(g_pRPCServer->m_pwcsDestination))
	{
		SysReAllocString(ppwcsDestination, g_pRPCServer->m_pwcsDestination);
		SysFreeString(destination);
	}

	return RPC_S_OK;
}
RPC_STATUS Imp_NavigateSpaceship()
{
	if (!g_pRPCServer)
		return RPC_E_FAULT;

	if (!g_pRPCServer->m_pwcsDestination)
		return RPC_E_FAULT;

	if (g_pRPCServer->m_hLocationEvent == INVALID_HANDLE_VALUE)
		return RPC_E_FAULT;

	if (IsEventStateSignaled(g_pRPCServer->m_hLocationEvent))
	{
		SERVER_OUT("On going navigation\n");
		return RPC_E_CALL_REJECTED;
	}

	return SetEvent(g_pRPCServer->m_hLocationEvent) ? RPC_S_OK : RPC_E_FAULT;
}
RPC_STATUS Imp_StopNavigation()
{
	if (!g_pRPCServer)
		return RPC_E_FAULT;

	if (g_pRPCServer->m_hLocationEvent == INVALID_HANDLE_VALUE)
		return RPC_E_FAULT;

	//If event is signaled, set it to non-signaled
	if (IsEventStateSignaled(g_pRPCServer->m_hLocationEvent))
	{
		ResetEvent(g_pRPCServer->m_hLocationEvent);
		SERVER_OUT("Spaceship stopped!\n");
		return RPC_S_OK;
	}
	else
	{
		SERVER_OUT("Spaceship already stopped!\n");
		return RPC_E_CALL_REJECTED;
	}

	return RPC_E_FAULT;
}
RPC_STATUS Imp_GetAvailableDestinations(BSTR* ppwcsDestinations)
{
	if (!g_pRPCServer)
		return RPC_E_FAULT;

	std::wstring wsRetvals(L"");
	unsigned int i = 1;

	for (auto& item : g_pRPCServer->m_AvailableDestinations)
	{
		wsRetvals += L"\t[*]";
		wsRetvals += (const wchar_t*)item.first;
		wsRetvals += L"\n";
		i++;
	}
	SysReAllocString(ppwcsDestinations, wsRetvals.c_str());


	return RPC_S_OK;
}


//------------------------------Explicit binding functions-----------------------------
void Exp_InitializeNavigationServer(handle_t hBinding)
{
	if (!g_pRPCServer)
	{
		g_pRPCServer = new RPCServer();
		if (g_pRPCServer)
		{
			if (g_pRPCServer->m_hStopEvent == INVALID_HANDLE_VALUE)
				g_pRPCServer->m_hStopEvent = CreateEventW(NULL, TRUE, TRUE, NULL); //Initial state: signaled

			if (g_pRPCServer->m_hLocationEvent == INVALID_HANDLE_VALUE)
				//Creates Location monitor non-signaled state event to trigger in LocationThread
				g_pRPCServer->m_hLocationEvent = CreateEventW(NULL, TRUE, FALSE, NULL); //Initial state: non-signaled

			if (g_pRPCServer->m_hLocationThread == INVALID_HANDLE_VALUE)
				//Create location thread
				g_pRPCServer->m_hLocationThread = CreateThread(0, 0, LocationThread, g_pRPCServer, 0, 0);

			SERVER_OUT("Navigation server is ready.\n");
		}
	}
};
void Exp_TerminateNavigationServer(handle_t hBinding)
{
	SERVER_OUT("Closing navigation server via explicit shutdown..\n");

	RPC_STATUS rpcstatus = RpcMgmtStopServerListening(NULL);

	if (rpcstatus == RPC_S_OK)
	{
		if (g_pRPCServer)
		{
			delete g_pRPCServer;
			g_pRPCServer = nullptr;
			
		}
		SERVER_OUT("Navigation server terminated!\n");
	}
	else
	{
		SERVER_OUT("Failed to stop server! %ld\n",rpcstatus);
	}
}

RPC_STATUS Exp_GetCurrentLocation(handle_t hBinding, Coordinates* pLocation)
{
	if (!g_pRPCServer || !hBinding)
		return RPC_E_FAULT;

	*pLocation = g_pRPCServer->m_Coordinates;

	return RPC_S_OK;
};
RPC_STATUS Exp_SetDestination(handle_t hBinding, BSTR pwcsDestination)
{
	if (!g_pRPCServer || !pwcsDestination || !hBinding)
		return RPC_E_FAULT;

	if (g_pRPCServer->m_hLocationEvent == INVALID_HANDLE_VALUE)
		return RPC_E_FAULT;

	bool bSignaled = false;
	bool bFound = false;

	//Check event state
	if (IsEventStateSignaled(g_pRPCServer->m_hLocationEvent))
	{
		SERVER_OUT("On going navigation\n");
		return RPC_E_CALL_REJECTED;
	}


	AvailableDestinations& DestinationList = g_pRPCServer->m_AvailableDestinations;

	for (auto& Destination : DestinationList)
	{
		if (_wcsicmp(Destination.first, pwcsDestination) == 0)
		{
			g_pRPCServer->SetDestination(pwcsDestination);
			SERVER_OUT("Destination set to %ls\n", pwcsDestination);
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		SERVER_OUT("%ls not an available destination!\n", pwcsDestination);
		return RPC_E_FAULT;
	}

	return RPC_S_OK;
};
RPC_STATUS Exp_GetDestination(handle_t hBinding, BSTR* ppwcsDestination)
{
	if (!g_pRPCServer || !ppwcsDestination || !hBinding)
		return RPC_E_FAULT;

	if (!g_pRPCServer->m_pwcsDestination)
		return RPC_E_FAULT;

	if (BSTR destination = SysAllocString(g_pRPCServer->m_pwcsDestination))
	{
		SysReAllocString(ppwcsDestination, g_pRPCServer->m_pwcsDestination);
		SysFreeString(destination);
	}

	return RPC_S_OK;
}
RPC_STATUS Exp_NavigateSpaceship(handle_t hBinding)
{
	if (!g_pRPCServer || !hBinding)
		return RPC_E_FAULT;

	if (!g_pRPCServer->m_pwcsDestination)
		return RPC_E_FAULT;

	if (g_pRPCServer->m_hLocationEvent == INVALID_HANDLE_VALUE)
		return RPC_E_FAULT;

	if (IsEventStateSignaled(g_pRPCServer->m_hLocationEvent))
	{
		SERVER_OUT("On going navigation\n");
		return RPC_E_CALL_REJECTED;
	}

	return SetEvent(g_pRPCServer->m_hLocationEvent) ? RPC_S_OK : RPC_E_FAULT;
}
RPC_STATUS Exp_StopNavigation(handle_t hBinding)
{
	if (!g_pRPCServer || !hBinding)
		return RPC_E_FAULT;

	if (g_pRPCServer->m_hLocationEvent == INVALID_HANDLE_VALUE)
		return RPC_E_FAULT;

	//If event is signaled, set it to non-signaled
	if (IsEventStateSignaled(g_pRPCServer->m_hLocationEvent))
	{
		ResetEvent(g_pRPCServer->m_hLocationEvent);
		SERVER_OUT("Spaceship stopped!\n");
		return RPC_S_OK;
	}
	else
	{
		SERVER_OUT("Spaceship already stopped!\n");
		return RPC_E_CALL_REJECTED;
	}

	return RPC_E_FAULT;
}
RPC_STATUS Exp_GetAvailableDestinations(handle_t hBinding, BSTR* ppwcsDestinations)
{
	if (!g_pRPCServer || !hBinding)
		return RPC_E_FAULT;

	std::wstring wsRetvals(L"");
	unsigned int i = 1;

	for (auto& item : g_pRPCServer->m_AvailableDestinations)
	{
		wsRetvals += L"\t[*]";
		wsRetvals += (const wchar_t*)item.first;
		wsRetvals += L"\n";
		i++;
	}
	SysReAllocString(ppwcsDestinations, wsRetvals.c_str());

	return RPC_S_OK;
}


//-------------------------------------Server utils------------------------------------
void PrintRPCServer()
{
	std::wcout <<
		R"(
  _____  _____   _____  _____                          
 |  __ \|  __ \ / ____|/ ____|                         
 | |__) | |__) | |    | (___   ___ _ ____   _____ _ __ 
 |  _  /|  ___/| |     \___ \ / _ \ '__\ \ / / _ \ '__|
 | | \ \| |    | |____ ____) |  __/ |   \ V /  __/ |   
 |_|  \_\_|     \_____|_____/ \___|_|    \_/ \___|_|   
#######################################################
Made by Matan Shitrit (0xMegaByte) 2022.
#######################################################
        )" << "\n";
}
void ServerDebugOutput(const char* format, ...)
{
	printf("[server] ");
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
RPC_STATUS __stdcall SecurityCallback(RPC_IF_HANDLE /*hInterface*/, void* /*pBindingHandle*/)
{
	//Always allow anyone.
	SERVER_OUT("%s: Called!\n", __FUNCTION__);
	return RPC_S_OK;
}