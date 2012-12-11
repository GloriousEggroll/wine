/*
 * Copyright 2005 Jacek Caban
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define COBJMACROS

#include "oaidl.h"
#include "rpcproxy.h"
#include "atlbase.h"

#include "wine/debug.h"
#include "wine/unicode.h"

WINE_DEFAULT_DEBUG_CHANNEL(atl);

/**************************************************************
 * ClassFactory implementation
 */

static HRESULT WINAPI RegistrarCF_QueryInterface(IClassFactory *iface, REFIID riid, void **ppvObject)
{
    TRACE("(%p)->(%s %p)\n", iface, debugstr_guid(riid), ppvObject);

    if(IsEqualGUID(&IID_IUnknown, riid) || IsEqualGUID(&IID_IClassFactory, riid)) {
        *ppvObject = iface;
        IClassFactory_AddRef( iface );
        return S_OK;
    }

    return E_NOINTERFACE;
}

static ULONG WINAPI RegistrarCF_AddRef(IClassFactory *iface)
{
    return 2;
}

static ULONG WINAPI RegistrarCF_Release(IClassFactory *iface)
{
    return 1;
}

static HRESULT WINAPI RegistrarCF_CreateInstance(IClassFactory *iface, LPUNKNOWN pUnkOuter,
                                                REFIID riid, void **ppv)
{
    IRegistrar *registrar;
    HRESULT hres;

    TRACE("(%p)->(%s %p)\n", iface, debugstr_guid(riid), ppv);

    if(pUnkOuter) {
        *ppv = NULL;
        return CLASS_E_NOAGGREGATION;
    }

    hres = AtlCreateRegistrar(&registrar);
    if(FAILED(hres))
        return hres;

    hres = IRegistrar_QueryInterface(registrar, riid, ppv);
    IRegistrar_Release(registrar);
    return hres;
}

static HRESULT WINAPI RegistrarCF_LockServer(IClassFactory *iface, BOOL lock)
{
    TRACE("(%p)->(%x)\n", iface, lock);
    return S_OK;
}

static const IClassFactoryVtbl IRegistrarCFVtbl = {
    RegistrarCF_QueryInterface,
    RegistrarCF_AddRef,
    RegistrarCF_Release,
    RegistrarCF_CreateInstance,
    RegistrarCF_LockServer
};

static IClassFactory RegistrarCF = { &IRegistrarCFVtbl };

/**************************************************************
 * DllGetClassObject (ATL.2)
 */
HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID riid, LPVOID *ppvObject)
{
    TRACE("(%s %s %p)\n", debugstr_guid(clsid), debugstr_guid(riid), ppvObject);

    if(IsEqualGUID(&CLSID_Registrar, clsid))
        return IClassFactory_QueryInterface( &RegistrarCF, riid, ppvObject );

    FIXME("Not supported class %s\n", debugstr_guid(clsid));
    return CLASS_E_CLASSNOTAVAILABLE;
}

extern HINSTANCE hInst;

/***********************************************************************
 *           AtlModuleUpdateRegistryFromResourceD         [ATL.@]
 *
 */
HRESULT WINAPI AtlModuleUpdateRegistryFromResourceD(_ATL_MODULEW* pM, LPCOLESTR lpszRes,
        BOOL bRegister, struct _ATL_REGMAP_ENTRY* pMapEntries, IRegistrar* pReg)
{
    TRACE("(%p %s %d %p %p)\n", pM, debugstr_w(lpszRes), bRegister, pMapEntries, pReg);

    return AtlUpdateRegistryFromResourceD(pM->m_hInst, lpszRes, bRegister, pMapEntries, pReg);
}

/***********************************************************************
 *              DllRegisterServer (ATL.@)
 */
HRESULT WINAPI DllRegisterServer(void)
{
    return __wine_register_resources( hInst );
}

/***********************************************************************
 *              DllUnRegisterServer (ATL.@)
 */
HRESULT WINAPI DllUnregisterServer(void)
{
    return __wine_unregister_resources( hInst );
}

/***********************************************************************
 *              DllCanUnloadNow (ATL.@)
 */
HRESULT WINAPI DllCanUnloadNow(void)
{
    return S_FALSE;
}
