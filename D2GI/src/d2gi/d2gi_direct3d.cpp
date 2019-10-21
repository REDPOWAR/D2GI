
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
	*lpDev = (IDirect3DDevice7*)new D2GIDevice(m_pD2GI);

	return DD_OK;
}


HRESULT D2GIDirect3D::EnumDevices(LPD3DENUMDEVICESCALLBACK7 pCallback, LPVOID pArg)
{
	INT i;

	for (i = 0; i < (INT)g_uDeviceCount; i++)
	{
		if (!pCallback(g_lpszDeviceDescs[i], g_lpszDeviceNames[i], g_asDeviceDescs + i, pArg))
			break;
	}

	return D3D_OK;
}


HRESULT D2GIDirect3D::EnumZBufferFormats(REFCLSID, D3D7::LPD3DENUMPIXELFORMATSCALLBACK pCallback, LPVOID lpArg)
{
	INT i;

	for (i = 0; i < (INT)g_uZBufferFormatsCount; i++)
	{
		if (!pCallback(g_asZBufferFormats + i, lpArg))
			break;
	}

	return DD_OK;
}
