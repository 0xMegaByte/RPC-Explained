#pragma once
#include <iostream>
#include "..\IDL\RPC-Explained.h"
#include <unordered_map>
#include <format>
#define CHECK( status ) if(status){ exit(status);}

#define SERVER_OUT(str,...) ServerDebugOutput(str,__VA_ARGS__)

typedef std::unordered_map<const wchar_t*, Coordinates> AvailableDestinations;

class RPCServer
{
public:
	RPCServer();
	~RPCServer();

	AvailableDestinations	m_AvailableDestinations;
	BSTR					m_pwcsDestination;
	HANDLE					m_hLocationThread;
	HANDLE					m_hLocationEvent;
	HANDLE					m_hStopEvent;
	Coordinates				m_Coordinates;

	void SetDestination(BSTR pwcsDestination);
};

DWORD WINAPI LocationThread(LPVOID lpv);

bool IsEventStateSignaled(HANDLE hEvent);


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//=+		   				  IDL Functions Implementations							 =+
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

//------------------------------Implicit binding functions-----------------------------
void Imp_InitializeNavigationServer();
void Imp_TerminateNavigationServer();

RPC_STATUS Imp_GetCurrentLocation(Coordinates* pLocation);
RPC_STATUS Imp_SetDestination(BSTR pwcsDestination);
RPC_STATUS Imp_GetDestination(BSTR* ppwcsDestination);
RPC_STATUS Imp_NavigateSpaceship();
RPC_STATUS Imp_StopNavigation();
RPC_STATUS Imp_GetAvailableDestinations(BSTR* ppwcDestinations);


//------------------------------Explicit binding functions-----------------------------
void Exp_InitializeNavigationServer(handle_t hBinding);
void Exp_TerminateNavigationServer(handle_t hBinding);

RPC_STATUS Exp_GetCurrentLocation(handle_t hBinding, Coordinates* pLocation);
RPC_STATUS Exp_SetDestination(handle_t hBinding, BSTR pwcsDestination);
RPC_STATUS Exp_GetDestination(handle_t hBinding, BSTR* ppwcsDestination);
RPC_STATUS Exp_NavigateSpaceship(handle_t hBinding);
RPC_STATUS Exp_StopNavigation(handle_t hBinding);
RPC_STATUS Exp_GetAvailableDestinations(handle_t hBinding, BSTR* ppwcsDestinations);


//-------------------------------------Server utils------------------------------------
void PrintRPCServer();
void ServerDebugOutput(const char* format, ...);
RPC_STATUS __stdcall SecurityCallback(RPC_IF_HANDLE /*hInterface*/, void* /*pBindingHandle*/);