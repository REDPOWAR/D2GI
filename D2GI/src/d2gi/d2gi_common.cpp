
#include "d2gi_common.h"
#include "d2gi.h"


D3D9::IDirect3D9* D2GIBase::GetD3D9()
{
	return m_pD2GI->GetD3D9();
}


D3D9::IDirect3DDevice9* D2GIBase::GetD3D9Device()
{
	return m_pD2GI->GetD3D9Device();
}
