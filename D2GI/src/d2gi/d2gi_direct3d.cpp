
#include "../common.h"

#include "d2gi_direct3d.h"
#include "d2gi_surface.h"
#include "d2gi_device.h"


using namespace D3D7;


D2GIDirect3D::D2GIDirect3D(IDirect3D7* pOriginal) : D3DProxy(pOriginal)
{

}


D2GIDirect3D::~D2GIDirect3D()
{

}


HRESULT D2GIDirect3D::CreateDevice(REFCLSID iid, LPDIRECTDRAWSURFACE7 lpSurf, LPDIRECT3DDEVICE7* lpDev)
{
	Debug(TEXT("Creating device for 0x%08x surface"), ((D2GISurface*)lpSurf)->GetOriginal());
	HRESULT hRes = D3DProxy::CreateDevice(iid, ((D2GISurface*)lpSurf)->GetOriginal(), lpDev);

	if (SUCCEEDED(hRes))
		*lpDev = (IDirect3DDevice7*)new D2GIDevice((IDirect3DDevice7*)*lpDev);

	return hRes;
}
