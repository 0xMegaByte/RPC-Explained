

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 05:14:07 2038
 */
/* Compiler settings for .\RPC-Explained.idl, .\RPC-Explained.acf:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __RPC2DExplained_h__
#define __RPC2DExplained_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_RPC2DExplained_0000_0000 */
/* [local] */ 

typedef struct _Coordinates
    {
    double m_ldLongitude;
    double m_ldLatitude;
    } 	Coordinates;

typedef long RPC_STATUS;



extern RPC_IF_HANDLE __MIDL_itf_RPC2DExplained_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_RPC2DExplained_0000_0000_v0_0_s_ifspec;

#ifndef __ImplicitNavigation_INTERFACE_DEFINED__
#define __ImplicitNavigation_INTERFACE_DEFINED__

/* interface ImplicitNavigation */
/* [implicit_handle][version][uuid] */ 

void Imp_InitializeNavigationServer( void);

void Imp_TerminateNavigationServer( void);

RPC_STATUS Imp_GetCurrentLocation( 
    /* [retval][out] */ Coordinates *pLocation);

RPC_STATUS Imp_SetDestination( 
    /* [string][in] */ BSTR pwcsDestination);

RPC_STATUS Imp_GetDestination( 
    /* [retval][out] */ BSTR *ppwcsDestination);

RPC_STATUS Imp_NavigateSpaceship( void);

RPC_STATUS Imp_StopNavigation( void);

RPC_STATUS Imp_GetAvailableDestinations( 
    /* [retval][out] */ BSTR *ppwcsDestinations);


extern handle_t hImplicitBinding;


extern RPC_IF_HANDLE ImplicitNavigation_v1_0_c_ifspec;
extern RPC_IF_HANDLE ImplicitNavigation_v1_0_s_ifspec;
#endif /* __ImplicitNavigation_INTERFACE_DEFINED__ */

#ifndef __ExplicitNavigation_INTERFACE_DEFINED__
#define __ExplicitNavigation_INTERFACE_DEFINED__

/* interface ExplicitNavigation */
/* [explicit_handle][version][uuid] */ 

void Exp_InitializeNavigationServer( 
    /* [in] */ handle_t hBinding);

void Exp_TerminateNavigationServer( 
    /* [in] */ handle_t hBinding);

RPC_STATUS Exp_GetCurrentLocation( 
    /* [in] */ handle_t hBinding,
    /* [retval][out] */ Coordinates *pLocation);

RPC_STATUS Exp_SetDestination( 
    /* [in] */ handle_t hBinding,
    /* [string][in] */ BSTR pwcsDestination);

RPC_STATUS Exp_GetDestination( 
    /* [in] */ handle_t hBinding,
    /* [retval][out] */ BSTR *ppwcsDestination);

RPC_STATUS Exp_NavigateSpaceship( 
    /* [in] */ handle_t hBinding);

RPC_STATUS Exp_StopNavigation( 
    /* [in] */ handle_t hBinding);

RPC_STATUS Exp_GetAvailableDestinations( 
    /* [in] */ handle_t hBinding,
    /* [retval][out] */ BSTR *ppwcsDestinations);



extern RPC_IF_HANDLE ExplicitNavigation_v1_0_c_ifspec;
extern RPC_IF_HANDLE ExplicitNavigation_v1_0_s_ifspec;
#endif /* __ExplicitNavigation_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


