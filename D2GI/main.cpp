
#include <ddraw.h>
#include <tchar.h>

#define EXPORT extern "C"


HMODULE g_hDDLib = NULL;

HRESULT (WINAPI *g_pDirectDrawCreateEx)(GUID*, LPVOID*, REFIID, IUnknown*);
_Check_return_ HRESULT (WINAPI *g_pDirectDrawEnumerateA)(LPDDENUMCALLBACKA, LPVOID);


VOID LoadDDrawLib()
{
	if (g_hDDLib != NULL)
		return;

	TCHAR szPath[MAX_PATH];

	GetSystemDirectory(szPath, MAX_PATH);
	_tcscat(szPath, TEXT("\\ddraw.dll"));

	g_hDDLib = LoadLibrary(szPath);

	g_pDirectDrawCreateEx = (HRESULT(WINAPI*)(GUID*, LPVOID*, REFIID, IUnknown*))GetProcAddress(g_hDDLib, "DirectDrawCreateEx");
	g_pDirectDrawEnumerateA = (_Check_return_ HRESULT(WINAPI*)(LPDDENUMCALLBACKA, LPVOID))GetProcAddress(g_hDDLib, "DirectDrawEnumerateA");
}


EXPORT HRESULT WINAPI DirectDrawCreateEx(GUID* pGUID, LPVOID* lpDD, REFIID iid, IUnknown* pUnknown)
{
	LoadDDrawLib();

	return g_pDirectDrawCreateEx(pGUID, lpDD, iid, pUnknown);
}


EXPORT _Check_return_ HRESULT WINAPI DirectDrawEnumerateA(LPDDENUMCALLBACKA pCallback, LPVOID pContext)
{
	LoadDDrawLib();

	return g_pDirectDrawEnumerateA(pCallback, pContext);
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
	return DD_FALSE;
}



EXPORT _Check_return_ HRESULT WINAPI DirectDrawCreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR* lplpDDClipper, IUnknown FAR* pUnkOuter)
{
	return DD_FALSE;
}



EXPORT HRESULT WINAPI DirectDrawEnumerateExA(LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags)
{
	return DD_FALSE;
}



EXPORT HRESULT WINAPI DirectDrawEnumerateExW(LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags)
{
	return DD_FALSE;
}



EXPORT _Check_return_ HRESULT WINAPI DirectDrawEnumerateW(LPDDENUMCALLBACKW lpCallback, LPVOID lpContext)
{
	return DD_FALSE;
}



STDAPI DllCanUnloadNow()
{
	return DD_FALSE;
}



STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
	return DD_FALSE;
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


