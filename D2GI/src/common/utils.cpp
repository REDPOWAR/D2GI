
#include "common.h"
#include "utils.h"

#include <algorithm>

using namespace D3D7;


UINT CalcFVFStride(DWORD dwFVF)
{
	UINT uVertexStride = 0;

	if (dwFVF & ~(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | 
		D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_RESERVED1 | D3DFVF_XYZRHW))
		return 0;

	if (dwFVF & D3DFVF_XYZ)
		uVertexStride += sizeof(FLOAT) * 3;

	if (dwFVF & D3DFVF_XYZRHW)
		uVertexStride += sizeof(FLOAT) * 4;

	if (dwFVF & D3DFVF_NORMAL)
		uVertexStride += sizeof(FLOAT) * 3;

	if (dwFVF & D3DFVF_RESERVED1)
		uVertexStride += sizeof(DWORD);

	if (dwFVF & D3DFVF_DIFFUSE)
		uVertexStride += sizeof(DWORD);

	if (dwFVF & D3DFVF_SPECULAR)
		uVertexStride += sizeof(DWORD);

	uVertexStride += CalcFVFTextureCount(dwFVF) * sizeof(FLOAT) * 2;

	return uVertexStride;
}


UINT CalcPrimitiveCount(D3D7::D3DPRIMITIVETYPE pt, DWORD dwVertexOrIndexCount)
{
	switch (pt)
	{
		case D3D7::D3DPT_TRIANGLELIST:
			return dwVertexOrIndexCount / 3;
		case D3D7::D3DPT_TRIANGLEFAN:
		case D3D7::D3DPT_TRIANGLESTRIP:
			return dwVertexOrIndexCount - 2;
		case D3D7::D3DPT_POINTLIST:
			return dwVertexOrIndexCount;
		case D3D7::D3DPT_LINELIST:
			return dwVertexOrIndexCount / 2;
		case D3D7::D3DPT_LINESTRIP:
			return dwVertexOrIndexCount - 1;
	}

	return 0;
}


UINT CalcFVFTextureCount(DWORD dwFVF)
{
	return (dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
}


VOID CalcMipMapLevelSize(DWORD dwTextureW, DWORD dwTextureH, UINT uLevel, DWORD* pMipMapW, DWORD* pMipMapH)
{
	UINT uPow = 1 << uLevel;

	*pMipMapW = std::max(dwTextureW / uPow, 1ul);
	*pMipMapH = std::max(dwTextureH / uPow, 1ul);
}
