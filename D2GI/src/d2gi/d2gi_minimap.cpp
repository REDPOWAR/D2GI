#include "d2gi_minimap.h"

#include "../common/logger.h"
#include "d2gi.h"

#include "shaders/d2gi_minimap_ps.h"
#include "shaders/d2gi_minimap_vs.h"

#include "DirectXMath.h"

using namespace D3D9;

void D2GIMinimapRenderer::LoadResources(bool bResettingDevice)
{
	IDirect3DDevice9* pDev = GetD3D9Device();

	if (!m_VDecl)
	{
		const D3DVERTEXELEMENT9 asVertexElements[] =
		{
			{0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
			D3DDECL_END()
		};

		if (FAILED(pDev->CreateVertexDeclaration(asVertexElements, &m_VDecl)))
			Logger::Error(TEXT("Failed to create minimap vertex declaration"));

		if (FAILED(pDev->CreateVertexShader(reinterpret_cast<const DWORD*>(g_MinimapVS), &m_VS)))
			Logger::Error(TEXT("Failed to create minimap vertex shader"));

		if (FAILED(pDev->CreatePixelShader(reinterpret_cast<const DWORD*>(g_MinimapPS), &m_PS)))
			Logger::Error(TEXT("Failed to create minimap pixel shader"));
	}

	if (FAILED(pDev->CreateVertexBuffer(MAX_NUM_VERTICES * sizeof(*m_LockedVertexData), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_VB, nullptr)))
		Logger::Error(TEXT("Failed to create minimap vertex buffer"));
}

void D2GIMinimapRenderer::ReleaseResources(bool bResettingDevice)
{
	if (!bResettingDevice)
	{
		m_VDecl.Reset();
		m_VS.Reset();
		m_PS.Reset();
	}
	m_VB.Reset();
}

void D2GIMinimapRenderer::BeginMinimapDraw()
{
	m_NumVertices = 0;
	m_DrawSetup = false;

	m_VB->Lock(0, 0, reinterpret_cast<void**>(&m_LockedVertexData), D3DLOCK_DISCARD|D3DLOCK_NOSYSLOCK);
}

void D2GIMinimapRenderer::EndMinimapDraw()
{
	Flush();

	if (m_DrawSetup)
	{
		D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

		pDev->SetRenderState(D3D9::D3DRS_ALPHABLENDENABLE, m_PreviousAlphaBlendState);
		pDev->SetRenderState(D3D9::D3DRS_FOGENABLE, m_PreviousFogState);

		pDev->SetVertexShader(nullptr);
		pDev->SetPixelShader(nullptr);
	}
}

void D2GIMinimapRenderer::AddMinimapLine(float x1, float y1, float x2, float y2, DWORD color)
{
	auto& vert1 = m_LockedVertexData[m_NumVertices++];
	vert1.XY[0] = x1;
	vert1.XY[1] = y1;
	vert1.Color = color;

	auto& vert2 = m_LockedVertexData[m_NumVertices++];
	vert2.XY[0] = x2;
	vert2.XY[1] = y2;
	vert2.Color = color;

	if (m_NumVertices >= MAX_NUM_VERTICES)
	{
		Flush();

		m_VB->Lock(0, 0, reinterpret_cast<void**>(&m_LockedVertexData), D3DLOCK_DISCARD|D3DLOCK_NOSYSLOCK);
	}
}

void D2GIMinimapRenderer::Flush()
{
	m_VB->Unlock();

	if (m_NumVertices > 0)
	{
		D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
		if (!m_DrawSetup)
		{
			pDev->GetRenderState(D3D9::D3DRS_ALPHABLENDENABLE, &m_PreviousAlphaBlendState);
			pDev->GetRenderState(D3D9::D3DRS_FOGENABLE, &m_PreviousFogState);

			pDev->SetRenderState(D3D9::D3DRS_ALPHABLENDENABLE, FALSE);
			pDev->SetRenderState(D3D9::D3DRS_FOGENABLE, FALSE);

			pDev->SetVertexDeclaration(m_VDecl.Get());
			pDev->SetVertexShader(m_VS.Get());
			pDev->SetPixelShader(m_PS.Get());

			pDev->SetStreamSource(0, m_VB.Get(), 0, sizeof(*m_LockedVertexData));

			// We need the full viewport dimensions, not just the rect used currently
			const LONG Width = m_Viewport.right;
			const LONG Height = m_Viewport.bottom;

			// Builds a matrix to transform screen-space vertex coordinates to clip-space inside the vertex shader
			const DirectX::XMMATRIX transform = DirectX::XMMatrixSet(
				2.0f / Width, 0.0f,           0.0f, 0.0f,
				0.0f,         -2.0f / Height, 0.0f, 0.0f,
				0.0f,         0.0f,           1.0f, 0.0f,
				-1.0f - 2.0f * (m_Viewport.left - 0.5f) / Width, 1.0f - 2.0f * (m_Viewport.top - 0.5f) / Height, 0.0f, 1.0f
			);
			pDev->SetVertexShaderConstantF(0, reinterpret_cast<const float*>(&transform), 4);

			m_DrawSetup = true;
		}

		const DWORD MaxPrimitiveCount = GetD2GI()->GetMaxPrimitiveCount();
		DWORD PrimitiveCount = m_NumVertices / 2;
		DWORD StartVertex = 0;
		while (PrimitiveCount > MaxPrimitiveCount)
		{
			pDev->DrawPrimitive(D3D9::D3DPT_LINELIST, StartVertex, MaxPrimitiveCount);

			PrimitiveCount -= MaxPrimitiveCount;
			StartVertex += 2 * MaxPrimitiveCount;
		}

		pDev->DrawPrimitive(D3D9::D3DPT_LINELIST, StartVertex, PrimitiveCount);
		m_NumVertices = 0;
	}
}
