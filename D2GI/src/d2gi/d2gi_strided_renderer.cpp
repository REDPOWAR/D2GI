
#include "../utils.h"
#include "../logger.h"

#include "d2gi_strided_renderer.h"
#include "d2gi_vb_container.h"
#include "d2gi_ib_container.h"


D2GIStridedPrimitiveRenderer::D2GIStridedPrimitiveRenderer(D2GI* pD2GI) : D2GIBase(pD2GI)
{
	m_pVBContainer = new D2GIVertexBufferContainer(pD2GI);
	m_pIBContainer = new D2GIIndexBufferContainer(pD2GI);
}


D2GIStridedPrimitiveRenderer::~D2GIStridedPrimitiveRenderer()
{
	DEL(m_pVBContainer);
	DEL(m_pIBContainer);
}


VOID D2GIStridedPrimitiveRenderer::ReleaseResource()
{
	m_pVBContainer->ReleaseResource();
	m_pIBContainer->ReleaseResource();
}


VOID D2GIStridedPrimitiveRenderer::LoadResource()
{
	m_pVBContainer->LoadResource();
	m_pIBContainer->LoadResource();
}


VOID D2GIStridedPrimitiveRenderer::OnPresentationFinished()
{
	m_pVBContainer->Clear();
	m_pIBContainer->Clear();
}


VOID D2GIStridedPrimitiveRenderer::DrawIndexedPrimitiveStrided(
	D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData, 
	DWORD dwCount, LPWORD pIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

	if (pt != D3D7::D3DPT_TRIANGLELIST)
		return;

	SetupVertexStream(dwFVF, pData, dwCount);
	
	VOID* pIBData;
	UINT uIdxOffset;

	if ((pIBData = m_pIBContainer->LockStreamingSpace(sizeof(UINT16) * dwIdxCount)) == NULL)
		Logger::Error(TEXT("Failed to continue index streaming"));
	CopyMemory(pIBData, pIdx, sizeof(UINT16) * dwIdxCount);
	m_pIBContainer->UnlockStreamingSpace();

	uIdxOffset = m_pIBContainer->SetAsSource();

	pDev->DrawIndexedPrimitive((D3D9::D3DPRIMITIVETYPE)pt, 0, 0, dwCount, uIdxOffset, dwIdxCount / 3);
}


VOID D2GIStridedPrimitiveRenderer::SetupVertexStream(
	DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData,
	DWORD dwCount)
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	UINT uVertexStride;


	uVertexStride = CalcFVFStride(dwFVF);

	if (dwFVF & ~(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEX2 | D3DFVF_DIFFUSE))
		return;


	VOID* pVBData;
	INT i, j;
	UINT uCurrentVertexStructOffset = 0;
	UINT uTextureCount = CalcFVFTextureCount(dwFVF);

	if((pVBData = m_pVBContainer->LockStreamingSpace(uVertexStride * dwCount)) == NULL)
		Logger::Error(TEXT("Failed to continue vertex streaming"));

	if (dwFVF & D3DFVF_XYZ)
	{
		for (i = 0; i < dwCount; i++)
		{
			CopyMemory((BYTE*)pVBData + i * uVertexStride + uCurrentVertexStructOffset,
				(BYTE*)pData->position.lpvData + i * pData->position.dwStride, sizeof(FLOAT) * 3);
		}

		uCurrentVertexStructOffset += sizeof(FLOAT) * 3;
	}

	if (dwFVF & D3DFVF_NORMAL)
	{
		for (i = 0; i < dwCount; i++)
		{
			CopyMemory((BYTE*)pVBData + i * uVertexStride + uCurrentVertexStructOffset,
				(BYTE*)pData->normal.lpvData + i * pData->normal.dwStride, sizeof(FLOAT) * 3);
		}

		uCurrentVertexStructOffset += sizeof(FLOAT) * 3;
	}

	if (dwFVF & D3DFVF_DIFFUSE)
	{
		for (i = 0; i < dwCount; i++)
		{
			CopyMemory((BYTE*)pVBData + i * uVertexStride + uCurrentVertexStructOffset,
				(BYTE*)pData->diffuse.lpvData + i * pData->diffuse.dwStride, sizeof(DWORD));
		}

		uCurrentVertexStructOffset += sizeof(DWORD);
	}

	for(i = 0; i < (INT)uTextureCount; i++)
	{
		for (j = 0; j < dwCount; j++)
		{
			CopyMemory((BYTE*)pVBData + j * uVertexStride + uCurrentVertexStructOffset,
				(BYTE*)pData->textureCoords[i].lpvData + j * pData->textureCoords[i].dwStride, sizeof(FLOAT) * 2);
		}

		uCurrentVertexStructOffset += sizeof(FLOAT) * 2;
	}

	m_pVBContainer->UnlockStreamingSpace();

	pDev->SetFVF(dwFVF);

	m_pVBContainer->SetAsSource(uVertexStride);
}


VOID D2GIStridedPrimitiveRenderer::DrawPrimitiveStrided(
	D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData,
	DWORD dwCount, DWORD dwFlags)
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	UINT uPrimCount;

	SetupVertexStream(dwFVF, pData, dwCount);

	uPrimCount = CalcPrimitiveCount(pt, dwCount);

	pDev->DrawPrimitive((D3D9::D3DPRIMITIVETYPE)pt, 0, uPrimCount);
}
