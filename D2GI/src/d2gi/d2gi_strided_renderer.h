#pragma once

#include "../common.h"
#include "../d3d9.h"
#include "../d3d7.h"

#include "d2gi_common.h"


class D2GIVertexBufferContainer;
class D2GIIndexBufferContainer;


class D2GIStridedPrimitiveRenderer : public D2GIBase
{
	D2GIVertexBufferContainer* m_pVBContainer;
	D2GIIndexBufferContainer* m_pIBContainer;

	VOID SetupVertexStream(DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData, DWORD dwCount);
public:
	D2GIStridedPrimitiveRenderer(D2GI*);
	~D2GIStridedPrimitiveRenderer();

	VOID ReleaseResource();
	VOID LoadResource();
	VOID OnPresentationFinished();


	VOID DrawIndexedPrimitiveStrided(D3D7::D3DPRIMITIVETYPE, DWORD, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA, DWORD, LPWORD, DWORD, DWORD);
	VOID DrawPrimitiveStrided(D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData, DWORD dwCount, DWORD dwFlags);
};
