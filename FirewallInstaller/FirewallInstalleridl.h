

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Wed Oct 08 11:17:02 2008
 */
/* Compiler settings for .\FirewallInstaller.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __FirewallInstalleridl_h__
#define __FirewallInstalleridl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DFirewallInstaller_FWD_DEFINED__
#define ___DFirewallInstaller_FWD_DEFINED__
typedef interface _DFirewallInstaller _DFirewallInstaller;
#endif 	/* ___DFirewallInstaller_FWD_DEFINED__ */


#ifndef ___DFirewallInstallerEvents_FWD_DEFINED__
#define ___DFirewallInstallerEvents_FWD_DEFINED__
typedef interface _DFirewallInstallerEvents _DFirewallInstallerEvents;
#endif 	/* ___DFirewallInstallerEvents_FWD_DEFINED__ */


#ifndef __FirewallInstaller_FWD_DEFINED__
#define __FirewallInstaller_FWD_DEFINED__

#ifdef __cplusplus
typedef class FirewallInstaller FirewallInstaller;
#else
typedef struct FirewallInstaller FirewallInstaller;
#endif /* __cplusplus */

#endif 	/* __FirewallInstaller_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __FirewallInstallerLib_LIBRARY_DEFINED__
#define __FirewallInstallerLib_LIBRARY_DEFINED__

/* library FirewallInstallerLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_FirewallInstallerLib;

#ifndef ___DFirewallInstaller_DISPINTERFACE_DEFINED__
#define ___DFirewallInstaller_DISPINTERFACE_DEFINED__

/* dispinterface _DFirewallInstaller */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DFirewallInstaller;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("8031A168-FD77-4C00-837F-91B076D83940")
    _DFirewallInstaller : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DFirewallInstallerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DFirewallInstaller * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DFirewallInstaller * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DFirewallInstaller * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DFirewallInstaller * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DFirewallInstaller * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DFirewallInstaller * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DFirewallInstaller * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DFirewallInstallerVtbl;

    interface _DFirewallInstaller
    {
        CONST_VTBL struct _DFirewallInstallerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DFirewallInstaller_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DFirewallInstaller_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DFirewallInstaller_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DFirewallInstaller_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DFirewallInstaller_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DFirewallInstaller_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DFirewallInstaller_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DFirewallInstaller_DISPINTERFACE_DEFINED__ */


#ifndef ___DFirewallInstallerEvents_DISPINTERFACE_DEFINED__
#define ___DFirewallInstallerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DFirewallInstallerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DFirewallInstallerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EA685E48-5268-49F3-86CF-33CD68DBD528")
    _DFirewallInstallerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DFirewallInstallerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DFirewallInstallerEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DFirewallInstallerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DFirewallInstallerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DFirewallInstallerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DFirewallInstallerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DFirewallInstallerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DFirewallInstallerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DFirewallInstallerEventsVtbl;

    interface _DFirewallInstallerEvents
    {
        CONST_VTBL struct _DFirewallInstallerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DFirewallInstallerEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DFirewallInstallerEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DFirewallInstallerEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DFirewallInstallerEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DFirewallInstallerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DFirewallInstallerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DFirewallInstallerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DFirewallInstallerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_FirewallInstaller;

#ifdef __cplusplus

class DECLSPEC_UUID("71CBE562-FCAE-4CEA-A153-837B8E20A31C")
FirewallInstaller;
#endif
#endif /* __FirewallInstallerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


