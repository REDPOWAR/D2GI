
#include "d2gi_direct3d.h"

#include "../d3d/d3d_device.h"


D2GIDirect3D::D2GIDirect3D(IDirect3D7* pOriginal) : D3DProxy(pOriginal)
{

}


D2GIDirect3D::~D2GIDirect3D()
{

}


HRESULT D2GIDirect3D::CreateDevice(REFCLSID iid, LPDIRECTDRAWSURFACE7 lpSurf, LPDIRECT3DDEVICE7* lpDev)
{
	HRESULT hRes = D3DProxy::CreateDevice(iid, lpSurf, lpDev);

	if (SUCCEEDED(hRes))
		*lpDev = (IDirect3DDevice7*)new DeviceProxy((IDirect3DDevice7*)*lpDev);

	return hRes;
}
