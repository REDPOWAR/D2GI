#pragma once

#include "d2gi_common.h"

#include <wrl/client.h>

#include <array>
#include <optional>

class D2GIMinimapRenderer : public D2GIBase
{
public:
	using D2GIBase::D2GIBase;

	void SetViewport(const RECT& viewport) { m_Viewport = viewport; }
	void BeginMinimapDraw();
	void EndMinimapDraw();
	void AddMinimapLine(int left, int top, int x1, int y1, int x2, int y2, DWORD color);

	void LoadResources();
	void ReleaseResources();

private:
	void Flush();

private:
	Microsoft::WRL::ComPtr<D3D9::IDirect3DVertexDeclaration9> m_VDecl;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DVertexShader9> m_VS;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DPixelShader9> m_PS;

	RECT m_Viewport;
	DWORD m_NumVertices = 0;
	bool m_DrawSetup = false;

	struct LineFVFData
	{
		float XYZRHW[2];
		D3D9::D3DCOLOR Color;
	};
	std::array<LineFVFData, 8192> m_LineVertexCache; // Arbitrary number
};
