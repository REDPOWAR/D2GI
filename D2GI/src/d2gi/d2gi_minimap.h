#pragma once

#include "d2gi_common.h"

#include <wrl/client.h>

class D2GIMinimapRenderer : public D2GIBase
{
public:
	using D2GIBase::D2GIBase;

	void SetViewport(const RECT& viewport) { m_Viewport = viewport; }
	void BeginMinimapDraw();
	void EndMinimapDraw();
	void AddMinimapLine(float x1, float y1, float x2, float y2, DWORD color);

	void LoadResources(bool bResettingDevice);
	void ReleaseResources(bool bResettingDevice);

private:
	void Flush();

private:
	static constexpr size_t MAX_NUM_VERTICES = 12 * 1024;

	Microsoft::WRL::ComPtr<D3D9::IDirect3DVertexDeclaration9> m_VDecl;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DVertexShader9> m_VS;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DPixelShader9> m_PS;

	Microsoft::WRL::ComPtr<D3D9::IDirect3DVertexBuffer9> m_VB;

	RECT m_Viewport;
	DWORD m_NumVertices = 0;

	DWORD m_PreviousAlphaBlendState = 0, m_PreviousFogState = 0;
	bool m_DrawSetup = false;

	struct LineFVFData
	{
		float XY[2];
		D3D9::D3DCOLOR Color;
	};
	LineFVFData* m_LockedVertexData = nullptr;
};
