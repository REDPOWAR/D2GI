
#include "../common/utils.h"
#include "../common/logger.h"

#include "d2gi_strided_renderer.h"
#include "d2gi_vb_container.h"
#include "d2gi_ib_container.h"


D2GIStridedPrimitiveRenderer::D2GIStridedPrimitiveRenderer(D2GI* pD2GI) : D2GIBase(pD2GI),
	m_VBContainer(pD2GI), m_IBContainer(pD2GI)
{
}


D2GIStridedPrimitiveRenderer::~D2GIStridedPrimitiveRenderer()
{
}


VOID D2GIStridedPrimitiveRenderer::ReleaseResource(bool bResettingDevice)
{
	m_VBContainer.ReleaseResource(bResettingDevice);
	m_IBContainer.ReleaseResource(bResettingDevice);
}


VOID D2GIStridedPrimitiveRenderer::LoadResource(bool bResettingDevice)
{
	m_VBContainer.LoadResource(bResettingDevice);
	m_IBContainer.LoadResource(bResettingDevice);
}


VOID D2GIStridedPrimitiveRenderer::DrawIndexedPrimitiveStrided(
	D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData, 
	DWORD dwCount, LPWORD pIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

	if (pt != D3D7::D3DPT_TRIANGLELIST)
		return;

	SetupVertexStream(dwFVF, pData, dwCount);
	
	const auto IBData = m_IBContainer.LockStreamingSpace(sizeof(UINT16) * dwIdxCount);

	if (!IBData)
		Logger::Error(TEXT("Failed to continue index streaming"));

	CopyMemory(IBData.Buffer, pIdx, sizeof(UINT16) * dwIdxCount);
	m_IBContainer.UnlockStreamingSpace();

	UINT uIdxOffset = m_IBContainer.SetAsSource(IBData);

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

	INT i, j;
	UINT uCurrentVertexStructOffset = 0;
	UINT uTextureCount = CalcFVFTextureCount(dwFVF);

	auto VBData = m_VBContainer.LockStreamingSpace(uVertexStride * dwCount);
	if(!VBData)
		Logger::Error(TEXT("Failed to continue vertex streaming"));

	if (dwFVF & D3DFVF_XYZ)
	{
		for (i = 0; i < (INT)dwCount; i++)
		{
			CopyMemory((BYTE*)VBData.Buffer + i * uVertexStride + uCurrentVertexStructOffset,
				(BYTE*)pData->position.lpvData + i * pData->position.dwStride, sizeof(FLOAT) * 3);
		}

		uCurrentVertexStructOffset += sizeof(FLOAT) * 3;
	}

	if (dwFVF & D3DFVF_NORMAL)
	{
		for (i = 0; i < (INT)dwCount; i++)
		{
			CopyMemory((BYTE*)VBData.Buffer + i * uVertexStride + uCurrentVertexStructOffset,
				(BYTE*)pData->normal.lpvData + i * pData->normal.dwStride, sizeof(FLOAT) * 3);
		}

		uCurrentVertexStructOffset += sizeof(FLOAT) * 3;
	}

	if (dwFVF & D3DFVF_DIFFUSE)
	{
		for (i = 0; i < (INT)dwCount; i++)
		{
			CopyMemory((BYTE*)VBData.Buffer + i * uVertexStride + uCurrentVertexStructOffset,
				(BYTE*)pData->diffuse.lpvData + i * pData->diffuse.dwStride, sizeof(DWORD));
		}

		uCurrentVertexStructOffset += sizeof(DWORD);
	}

	for(i = 0; i < (INT)uTextureCount; i++)
	{
		for (j = 0; j < (INT)dwCount; j++)
		{
			CopyMemory((BYTE*)VBData.Buffer + j * uVertexStride + uCurrentVertexStructOffset,
				(BYTE*)pData->textureCoords[i].lpvData + j * pData->textureCoords[i].dwStride, sizeof(FLOAT) * 2);
		}

		uCurrentVertexStructOffset += sizeof(FLOAT) * 2;
	}

	m_VBContainer.UnlockStreamingSpace();

	pDev->SetFVF(dwFVF);

	m_VBContainer.SetAsSource(VBData, uVertexStride);
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
