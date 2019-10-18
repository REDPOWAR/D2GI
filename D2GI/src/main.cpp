
#include <ddraw.h>

#include "d2gi/d2gi.h"


#define EXPORT extern "C"


EXPORT HRESULT WINAPI DirectDrawCreateEx(GUID* pGUID, LPVOID* lpDD, REFIID iid, IUnknown* pUnknown)
{
	D2GI* pD2GI = new D2GI();

	*lpDD = (IDirectDraw7*)pD2GI->GetDirectDrawProxy();

	return DD_OK;
}


EXPORT _Check_return_ HRESULT WINAPI DirectDrawEnumerateA(LPDDENUMCALLBACKA pCallback, LPVOID pContext)
{
	pCallback(NULL, "Primary Video Driver", "display", pContext);

	return DD_OK;
}


EXPORT VOID WINAPI AcquireDDThreadLock()
{

}


EXPORT VOID WINAPI ReleaseDDThreadLock()
{

}


EXPORT VOID WINAPI CompleteCreateSysmemSurface()
{

}



EXPORT VOID WINAPI D3DParseUnknownCommand()
{

}



EXPORT VOID WINAPI DDGetAttachedSurfaceLcl()
{

}



EXPORT VOID WINAPI DDInternalLock()
{

}



EXPORT VOID WINAPI DDInternalUnlock()
{

}



EXPORT VOID WINAPI DSoundHelp()
{

}



EXPORT _Check_return_ HRESULT WINAPI DirectDrawCreate(GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter)
{
	return DDERR_GENERIC;
}



EXPORT _Check_return_ HRESULT WINAPI DirectDrawCreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR* lplpDDClipper, IUnknown FAR* pUnkOuter)
{
	return DDERR_GENERIC;
}



EXPORT HRESULT WINAPI DirectDrawEnumerateExA(LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags)
{
	return DDERR_GENERIC;
}



EXPORT HRESULT WINAPI DirectDrawEnumerateExW(LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags)
{
	return DDERR_GENERIC;
}



EXPORT _Check_return_ HRESULT WINAPI DirectDrawEnumerateW(LPDDENUMCALLBACKW lpCallback, LPVOID lpContext)
{
	return DDERR_GENERIC;
}



STDAPI DllCanUnloadNow()
{
	return S_FALSE;
}



STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
	return S_FALSE;
}



EXPORT VOID WINAPI GetDDSurfaceLocal()
{

}



EXPORT VOID WINAPI GetOLEThunkData()
{

}



EXPORT VOID WINAPI GetSurfaceFromDC()
{

}



EXPORT VOID WINAPI RegisterSpecialCase()
{

}



EXPORT VOID WINAPI SetAppCompatData()
{

}


