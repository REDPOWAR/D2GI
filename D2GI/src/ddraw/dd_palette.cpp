
#include "dd_palette.h"


PaletteProxy::PaletteProxy() : Unknown()
{

}


PaletteProxy::~PaletteProxy()
{

}


HRESULT PaletteProxy::GetCaps(LPDWORD)
{
	return DDERR_GENERIC;
}


HRESULT PaletteProxy::GetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY)
{
	return DDERR_GENERIC;
}


HRESULT PaletteProxy::Initialize(D3D7::LPDIRECTDRAW, DWORD, LPPALETTEENTRY)
{
	return DDERR_GENERIC;
}


HRESULT PaletteProxy::SetEntries(DWORD, DWORD dwIdx, DWORD dwCount, LPPALETTEENTRY pEntries)
{
	return DDERR_GENERIC;
}
