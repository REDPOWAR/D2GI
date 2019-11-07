
#include "common.h"
#include "utils.h"

using namespace D3D7;


VOID DebugSurfaceDesc(DDSURFACEDESC2* pDesc)
{
	if (pDesc->dwFlags & DDSD_ALPHABITDEPTH)
		Debug(TEXT("DDSD_ALPHABITDEPTH: %i"), pDesc->dwAlphaBitDepth);

	if (pDesc->dwFlags & DDSD_BACKBUFFERCOUNT)
		Debug(TEXT("DDSD_BACKBUFFERCOUNT: %i"), pDesc->dwBackBufferCount);

	if (pDesc->dwFlags & DDSD_CAPS)
		Debug(TEXT("DDSD_CAPS: %i %i %i %i %i"),
			pDesc->ddsCaps.dwCaps, pDesc->ddsCaps.dwCaps2, pDesc->ddsCaps.dwCaps3,
			pDesc->ddsCaps.dwCaps4, pDesc->ddsCaps.dwVolumeDepth);

	if (pDesc->dwFlags & DDSD_CKDESTBLT)
		Debug(TEXT("DDSD_CKDESTBLT: %i"), pDesc->ddckCKDestBlt);

	if (pDesc->dwFlags & DDSD_CKDESTOVERLAY)
		Debug(TEXT("DDSD_CKDESTOVERLAY: %i"), pDesc->ddckCKDestOverlay);

	if (pDesc->dwFlags & DDSD_CKSRCBLT)
		Debug(TEXT("DDSD_CKSRCBLT: %i"), pDesc->ddckCKSrcBlt);

	if (pDesc->dwFlags & DDSD_CKSRCOVERLAY)
		Debug(TEXT("DDSD_CKSRCOVERLAY: %i"), pDesc->ddckCKSrcOverlay);

	if (pDesc->dwFlags & DDSD_FVF)
		Debug(TEXT("DDSD_FVF: %i"), pDesc->dwFVF);

	if (pDesc->dwFlags & DDSD_WIDTH)
		Debug(TEXT("DDSD_WIDTH: %i"), pDesc->dwWidth);

	if (pDesc->dwFlags & DDSD_HEIGHT)
		Debug(TEXT("DDSD_HEIGHT: %i"), pDesc->dwHeight);

	if (pDesc->dwFlags & DDSD_LINEARSIZE)
		Debug(TEXT("DDSD_LINEARSIZE: %i"), pDesc->dwLinearSize);

	if (pDesc->dwFlags & DDSD_LPSURFACE)
		Debug(TEXT("DDSD_LPSURFACE: %i"), pDesc->lpSurface);

	if (pDesc->dwFlags & DDSD_MIPMAPCOUNT)
		Debug(TEXT("DDSD_MIPMAPCOUNT: %i"), pDesc->dwMipMapCount);

	if (pDesc->dwFlags & DDSD_PITCH)
		Debug(TEXT("DDSD_PITCH: %i"), pDesc->lPitch);

	if (pDesc->dwFlags & DDSD_REFRESHRATE)
		Debug(TEXT("DDSD_REFRESHRATE: %i"), pDesc->dwRefreshRate);

	if (pDesc->dwFlags & DDSD_SRCVBHANDLE)
		Debug(TEXT("DDSD_SRCVBHANDLE: %i"), pDesc->dwSrcVBHandle);

	if (pDesc->dwFlags & DDSD_TEXTURESTAGE)
		Debug(TEXT("DDSD_TEXTURESTAGE: %i"), pDesc->dwTextureStage);

	if (pDesc->dwFlags & DDSD_PIXELFORMAT)
	{
		DDPIXELFORMAT* ppf = &pDesc->ddpfPixelFormat;

		Debug(TEXT("DDSD_PIXELFORMAT:"));
		if (ppf->dwFlags & DDPF_ALPHA)
			Debug(TEXT("\tDDPF_ALPHA: TRUE"));

		if (ppf->dwFlags & DDPF_ALPHAPIXELS)
			Debug(TEXT("\tDDPF_ALPHAPIXELS: TRUE"));

		if (ppf->dwFlags & DDPF_ALPHAPREMULT)
			Debug(TEXT("\tDDPF_ALPHAPREMULT: TRUE"));

		if (ppf->dwFlags & DDPF_BUMPDUDV)
			Debug(TEXT("\tDDPF_BUMPDUDV: %i %i"), ppf->dwBumpDuBitMask, ppf->dwBumpDvBitMask);

		if (ppf->dwFlags & DDPF_COMPRESSED)
			Debug(TEXT("\tDDPF_COMPRESSED: TRUE"));

		if (ppf->dwFlags & DDPF_FOURCC)
			Debug(TEXT("\tDDPF_FOURCC: %i"), ppf->dwFourCC);

		if (ppf->dwFlags & DDPF_LUMINANCE)
			Debug(TEXT("\tDDPF_LUMINANCE: TRUE"));

		if (ppf->dwFlags & DDPF_PALETTEINDEXED1)
			Debug(TEXT("\tDDPF_PALETTEINDEXED1: TRUE"));

		if (ppf->dwFlags & DDPF_PALETTEINDEXED2)
			Debug(TEXT("\tDDPF_PALETTEINDEXED2: TRUE"));

		if (ppf->dwFlags & DDPF_PALETTEINDEXED4)
			Debug(TEXT("\tDDPF_PALETTEINDEXED4: TRUE"));

		if (ppf->dwFlags & DDPF_PALETTEINDEXED8)
			Debug(TEXT("\tDDPF_PALETTEINDEXED8: TRUE"));

		if (ppf->dwFlags & DDPF_PALETTEINDEXEDTO8)
			Debug(TEXT("\tDDPF_PALETTEINDEXEDTO8: TRUE"));

		if (ppf->dwFlags & DDPF_RGB)
			Debug(TEXT("\tDDPF_RGB: TRUE"));

		if (ppf->dwFlags & DDPF_RGBTOYUV)
			Debug(TEXT("\tDDPF_RGBTOYUV: TRUE"));

		if (ppf->dwFlags & DDPF_STENCILBUFFER)
			Debug(TEXT("\tDDPF_STENCILBUFFER: TRUE"));

		if (ppf->dwFlags & DDPF_YUV)
			Debug(TEXT("\tDDPF_YUV: TRUE"));

		if (ppf->dwFlags & DDPF_ZBUFFER)
			Debug(TEXT("\tDDPF_ZBUFFER: TRUE"));

		if (ppf->dwFlags & DDPF_ZPIXELS)
			Debug(TEXT("\tDDPF_ZPIXELS: TRUE"));
	}
}


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
	INT i;

	*pMipMapW = dwTextureW;
	*pMipMapH = dwTextureH;

	for (i = 0; i < uLevel && !(*pMipMapW == 1 && *pMipMapH == 1); i++)
	{
		*pMipMapW = max(*pMipMapW / 2, 1);
		*pMipMapH = max(*pMipMapH / 2, 1);
	}
}


VOID ScaleProjectionAspectRatio(D3D9::D3DXMATRIX* pMatrix, FLOAT fScale)
{
	FLOAT fAspectRatio = pMatrix->_22 / pMatrix->_11;

	pMatrix->_11 = pMatrix->_22 / (fAspectRatio * fScale);
}
