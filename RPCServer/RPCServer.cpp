// RPCServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include "RPCServer.h"

int main()
{
	PrintRPCServer();

	printf("[Welcome to Spaceship's Navigation System (Server)]\n");
	SERVER_OUT("Initializing...\n");

	//this function tells the RPC run-time library to use the specified protocol sequence 
	//combined with the specified endpoint for receiving remote procedure calls.

	CHECK(RpcServerUseProtseqEpA(
		(RPC_CSTR)"ncacn_ip_tcp", //Use TCP-IP protocol
		RPC_C_PROTSEQ_MAX_REQS_DEFAULT, //Max call default value (10)
		(RPC_CSTR)"4202", //TCP-IP Port
		NULL)); //No security 

	//Register both Explicit and/or Implicit interfaces
	CHECK(RpcServerRegisterIf2(
		ImplicitNavigation_v1_0_s_ifspec,    // Register ImplicitNavigation interface
		NULL,                                // Use the MIDL generated entry-point vector.
		NULL,                                // Use the MIDL generated entry-point vector.
		RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, // Forces use of security callback.
		RPC_C_LISTEN_MAX_CALLS_DEFAULT,      // Use default number of concurrent calls.
		(unsigned)-1,                        // Infinite max size of incoming data blocks.
		SecurityCallback));

	SERVER_OUT("ImplicitNavigation_v1_0_s_ifspec interface registered!\n");
	//TODO: Add here explicit registeration with flag

	CHECK(RpcServerRegisterIf2(
		ExplicitNavigation_v1_0_s_ifspec,
		NULL,
		NULL,
		RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH,
		RPC_C_LISTEN_MAX_CALLS_DEFAULT,
		(unsigned)-1,
		SecurityCallback));

	SERVER_OUT("ExplicitNavigation_v1_0_s_ifspec interface registered!\n");

	// Start to listen for remote procedure
	// calls for all registered interfaces.
	// This call will not return until
	// RpcMgmtStopServerListening is called.
	CHECK(RpcServerListen(
		1,                                   // Recommended minimum number of threads.
		RPC_C_LISTEN_MAX_CALLS_DEFAULT,      // Recommended maximum number of threads.
		FALSE));                              // Start listening now.


	CHECK(RpcServerUnregisterIf(
		ImplicitNavigation_v1_0_s_ifspec,
		NULL,
		0));

	SERVER_OUT("ImplicitNavigation_v1_0_s_ifspec interface registered!\n");

	CHECK(RpcServerUnregisterIf(
		ExplicitNavigation_v1_0_s_ifspec,
		NULL,
		0));
	SERVER_OUT("ExplicitNavigation_v1_0_s_ifspec interface registered!\n");

	Sleep(5000);
}

void* __RPC_USER midl_user_allocate(size_t size)
{
	return malloc(size);
}

// Memory deallocation function for RPC.
void __RPC_USER midl_user_free(void* p)
{
	free(p);
}








