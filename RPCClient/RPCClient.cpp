// RPCClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include "RPCClient.h"

int main()
{
	PrintRPCClient();

	wchar_t* pwcsStringBinding = NULL;
	handle_t hExplicitBInding = NULL;

	// Creates a string binding handle.
	CHECK(RpcStringBindingComposeW(NULL, // UUID to bind to
		(RPC_WSTR)L"ncacn_ip_tcp",
		(RPC_WSTR)L"localhost",
		(RPC_WSTR)L"4202",
		NULL,
		(RPC_WSTR*)&pwcsStringBinding));

	if (pwcsStringBinding)
	{
		CLIENT_OUT("Composed String: %ls\n", pwcsStringBinding);

		SetBindingType();

		if (bImplicit)
		{
			//hImplicitBinding defined in the IDL file
			CHECK(RpcBindingFromStringBindingW((RPC_WSTR)pwcsStringBinding, &hImplicitBinding));
		}
		else
		{
			CHECK(RpcBindingFromStringBindingW((RPC_WSTR)pwcsStringBinding, &hExplicitBInding));
		}
	}
	
	__try
	{ 
		if (bImplicit)
		{
			if (hImplicitBinding)
			{
				Imp_InitializeNavigationServer();

				UserInputLoop();

				Imp_InitializeNavigationServer();

			}
		}
		else
		{
			if (hExplicitBInding)
			{
				CHECK(RpcEpResolveBinding(hExplicitBInding, ExplicitNavigation_v1_0_c_ifspec));

				Exp_InitializeNavigationServer(hExplicitBInding);

				UserInputLoop(hExplicitBInding);

				Exp_InitializeNavigationServer(hExplicitBInding);
			}
		}
	}
	__except (1)
	{
		std::cerr << "Runtime reported exception " << RpcExceptionCode() << std::endl;
	}

	CHECK(RpcStringFreeW((RPC_WSTR*)&pwcsStringBinding));

	// Releases binding handle resources and disconnects from the server.
	if (bImplicit)
	{
		CHECK(RpcBindingFree(&hImplicitBinding));
	}
	else
	{
		CHECK(RpcBindingFree(&hExplicitBInding));
	}


	delete pwcsStringBinding;

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