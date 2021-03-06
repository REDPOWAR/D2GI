#pragma once

#include "../common/d3d9.h"

class D2GI;

class D2GIBase
{
protected:
	D2GI* m_pD2GI;
public:
	D2GIBase(D2GI* pD2GI) : m_pD2GI(pD2GI) {}

	D2GI* GetD2GI() { return m_pD2GI; }
	D3D9::IDirect3D9* GetD3D9();
	D3D9::IDirect3DDevice9* GetD3D9Device();
};
