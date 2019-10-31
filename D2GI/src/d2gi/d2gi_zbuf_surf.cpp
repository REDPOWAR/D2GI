
#include "d2gi_zbuf_surf.h"
#include "d2gi.h"


D2GIZBufferSurface::D2GIZBufferSurface(D2GI* pD2GI, 
	DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{

}


D2GIZBufferSurface::~D2GIZBufferSurface()
{

}


HRESULT D2GIZBufferSurface::IsLost()
{
	return DD_OK;
}
