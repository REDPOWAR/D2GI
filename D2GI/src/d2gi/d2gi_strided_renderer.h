#pragma once

#include "../common/common.h"
#include "../common/d3d7.h"

#include "d2gi_common.h"
#include "d2gi_ib_container.h"
#include "d2gi_vb_container.h"


class D2GIStridedPrimitiveRenderer : public D2GIBase
{
	D2GIVertexBufferContainer m_VBContainer;
	D2GIIndexBufferContainer m_IBContainer;

	VOID SetupVertexStream(DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData, DWORD dwCount);
public:
	D2GIStridedPrimitiveRenderer(D2GI*);
	~D2GIStridedPrimitiveRenderer();

	void ReleaseResource(bool bResettingDevice);
	void LoadResource(bool bResettingDevice);

	VOID DrawIndexedPrimitiveStrided(D3D7::D3DPRIMITIVETYPE, DWORD, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA, DWORD, LPWORD, DWORD, DWORD);
	VOID DrawPrimitiveStrided(D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData, DWORD dwCount, DWORD dwFlags);
};
