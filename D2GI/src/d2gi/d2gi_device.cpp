
#include "../common.h"

#include "d2gi_device.h"
#include "d2gi_surface.h"


using namespace D3D7;


D2GIDevice::D2GIDevice(D2GI* pD2GI) : DeviceProxy(), D2GIBase(pD2GI)
{

}


D2GIDevice::~D2GIDevice()
{

}


HRESULT D2GIDevice::SetTexture(DWORD i, LPDIRECTDRAWSURFACE7 lpTex)
{
	/*if (lpTex != NULL)
		lpTex = ((D2GISurface*)lpTex)->GetOriginal();

	return DeviceProxy::SetTexture(i, lpTex);*/

	return DDERR_GENERIC;
}