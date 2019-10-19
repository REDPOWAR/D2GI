
#include "../common.h"

#include "d2gi_direct3d.h"
#include "d2gi_surface.h"
#include "d2gi_device.h"
#include "d2gi_enums.h"

using namespace D3D7;


D2GIDirect3D::D2GIDirect3D(D2GI* pD2GI) : D3DProxy(), D2GIBase(pD2GI)
{

}


D2GIDirect3D::~D2GIDirect3D()
{

}


HRESULT D2GIDirect3D::CreateDevice(REFCLSID iid, LPDIRECTDRAWSURFACE7 lpSurf, LPDIRECT3DDEVICE7* lpDev)
{
	/*Debug(TEXT("Creating device for 0x%08x surface"), ((D2GISurface*)lpSurf)->GetOriginal());
	HRESULT hRes = D3DProxy::CreateDevice(iid, ((D2GISurface*)lpSurf)->GetOriginal(), lpDev);

	if (SUCCEEDED(hRes))
		*lpDev = (IDirect3DDevice7*)new D2GIDevice((IDirect3DDevice7*)*lpDev);

	return hRes;*/

	return DDERR_GENERIC;
}


HRESULT D2GIDirect3D::EnumDevices(LPD3DENUMDEVICESCALLBACK7 pCallback, LPVOID pArg)
{
	pCallback(g_szDeviceDesc, g_szDeviceName, &g_sDeviceDesc, pArg);

	return D3D_OK;
}
