
#include "d2gi_zbuf_surf.h"
#include "d2gi.h"


D2GIZBufferSurface::D2GIZBufferSurface(D2GI* pD2GI) : D2GISurface(pD2GI)
{

}


D2GIZBufferSurface::~D2GIZBufferSurface()
{

}


HRESULT D2GIZBufferSurface::IsLost()
{
	return DD_OK;
}
