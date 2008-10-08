

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_FirewallInstallerLib,0x2ADA0098,0x3A05,0x443F,0xB3,0x5A,0xFA,0xDF,0xEB,0xFB,0x72,0x81);


MIDL_DEFINE_GUID(IID, DIID__DFirewallInstaller,0x8031A168,0xFD77,0x4C00,0x83,0x7F,0x91,0xB0,0x76,0xD8,0x39,0x40);


MIDL_DEFINE_GUID(IID, DIID__DFirewallInstallerEvents,0xEA685E48,0x5268,0x49F3,0x86,0xCF,0x33,0xCD,0x68,0xDB,0xD5,0x28);


MIDL_DEFINE_GUID(CLSID, CLSID_FirewallInstaller,0x71CBE562,0xFCAE,0x4CEA,0xA1,0x53,0x83,0x7B,0x8E,0x20,0xA3,0x1C);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



