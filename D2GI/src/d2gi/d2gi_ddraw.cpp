
#define INITGUID
#include <d3d.h>

#include "d2gi_ddraw.h"

#include "../d3d/d3d.h"


D2GIDirectDraw::D2GIDirectDraw(IDirectDraw7* pOriginal) : DDrawProxy(pOriginal)
{

}


D2GIDirectDraw::~D2GIDirectDraw()
{

}


HRESULT D2GIDirectDraw::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	if (IsEqualIID(riid, IID_IDirect3D7))
	{
		HRESULT hRes = DDrawProxy::QueryInterface(riid, ppvObj);

		if (SUCCEEDED(hRes))
			*ppvObj = (IDirect3D7*)new D3DProxy((IDirect3D7*)*ppvObj);

		return hRes;
	}

	return DD_FALSE;
}
