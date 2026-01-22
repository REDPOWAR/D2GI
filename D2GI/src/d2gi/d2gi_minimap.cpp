#include "d2gi_minimap.h"

#include "d2gi.h"

void D2GIMinimapRenderer::LoadResources()
{

}

void D2GIMinimapRenderer::ReleaseResources()
{

}

void D2GIMinimapRenderer::BeginMinimapDraw()
{
	m_NumVertices = 0;
	m_DrawSetup = false;
}

void D2GIMinimapRenderer::EndMinimapDraw()
{
	Flush();
}

void D2GIMinimapRenderer::AddMinimapLine(int left, int top, int x1, int y1, int x2, int y2, DWORD color)
{
	using namespace D3D9;

	const float WidthScale = m_pD2GI->GetWidthScale();
	const float HeightScale = m_pD2GI->GetHeightScale();

	auto& vert1 = m_LineVertexCache[m_NumVertices++];
	vert1.XYZRHW[0] = (left + x1) * WidthScale;
	vert1.XYZRHW[1] = (top + y1) * HeightScale;
	vert1.XYZRHW[2] = 0.0f;
	vert1.XYZRHW[3] = 1.0f;
	vert1.Color = color;

	//vert1.Specular = D3DCOLOR_RGBA(0, 0, 0, 255);

	auto& vert2 = m_LineVertexCache[m_NumVertices++];
	vert2.XYZRHW[0] = (left + x2) * WidthScale;
	vert2.XYZRHW[1] = (top + y2) * HeightScale;
	vert2.XYZRHW[2] = 0.0f;
	vert2.XYZRHW[3] = 1.0f;
	vert2.Color = color;

	//vert2.Specular = D3DCOLOR_RGBA(0, 0, 0, 255);

	if (m_NumVertices >= m_LineVertexCache.size())
	{
		Flush();
	}
}

void D2GIMinimapRenderer::Flush()
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	if (!m_DrawSetup)
	{
		pDev->SetRenderState(D3D9::D3DRS_ALPHABLENDENABLE, FALSE);
		pDev->SetRenderState(D3D9::D3DRS_FOGENABLE, FALSE);
		pDev->SetTexture(0, nullptr);

		pDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

		m_DrawSetup = true;
	}
	pDev->DrawPrimitiveUP(D3D9::D3DPT_LINELIST, m_NumVertices / 2, m_LineVertexCache.data(), sizeof(decltype(m_LineVertexCache)::value_type));
	m_NumVertices = 0;
}
