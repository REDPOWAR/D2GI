
#include "common/logger.h"

#include "d2gi/d2gi.h"
#include "d2gi/d2gi_config.h"
#include "d2gi/d2gi_hooks.h"


#define EXPORT extern "C"


EXPORT HRESULT WINAPI D3D7::DirectDrawCreateEx(GUID* pGUID, LPVOID* lpDD, REFIID iid, IUnknown* pUnknown)
{
	D2GI* pD2GI = new D2GI();

	*lpDD = (D3D7::IDirectDraw7*)pD2GI->GetDirectDrawProxy();

	return DD_OK;
}


EXPORT _Check_return_ HRESULT WINAPI D3D7::DirectDrawEnumerateA(D3D7::LPDDENUMCALLBACKA pCallback, LPVOID pContext)
{
	pCallback(NULL, "Primary Video Driver", "display", pContext);

	return DD_OK;
}


EXPORT _Check_return_ HRESULT WINAPI D3D7::DirectDrawCreate(GUID FAR* lpGUID, D3D7::LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter)
{
	return DDERR_GENERIC;
}


EXPORT _Check_return_ HRESULT WINAPI D3D7::DirectDrawCreateClipper(DWORD dwFlags, D3D7::LPDIRECTDRAWCLIPPER FAR* lplpDDClipper, IUnknown FAR* pUnkOuter)
{
	return DDERR_GENERIC;
}


EXPORT HRESULT WINAPI D3D7::DirectDrawEnumerateExA(D3D7::LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


EXPORT HRESULT WINAPI D3D7::DirectDrawEnumerateExW(D3D7::LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


EXPORT _Check_return_ HRESULT WINAPI D3D7::DirectDrawEnumerateW(D3D7::LPDDENUMCALLBACKW lpCallback, LPVOID lpContext)
{
	return DDERR_GENERIC;
}


BOOL WINAPI DllMain(HINSTANCE, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Logger::Log(TEXT("D2GI %s module loaded"), D2GI_VERSION);
		D2GIConfig::ReadFromFile();
		D2GIHookInjector::InjectHooks();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
		Logger::Log(TEXT("D2GI module unloaded\n"));

	return TRUE;
}
