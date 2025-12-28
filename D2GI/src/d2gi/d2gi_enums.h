#pragma once

#include "../common/d3d7.h"

#include "d2gi_common.h"


enum D2GIPIXELFORMAT
{
	D2GIPF_UNKNOWN = -1,
	D2GIPF_8_PAL,
	D2GIPF_16_565,
	D2GIPF_16_1555,
	D2GIPF_16_4444,
	D2GIPF_16_V8U8,
	D2GIPF_16_DEPTH,
	D2GIPF_16_555,
};


extern const D3D7::DDSURFACEDESC2 g_asStdDisplayModes[];
extern const UINT g_uStdDisplayModesCount;

extern const D3D7::DDCAPS g_sHALCaps;
extern const D3D7::DDCAPS g_sHELCaps;

extern D3D7::D3DDEVICEDESC7 g_asDeviceDescs[];
extern CHAR* g_lpszDeviceDescs[];
extern CHAR* g_lpszDeviceNames[];
extern const UINT g_uDeviceCount;

extern D3D7::DDPIXELFORMAT g_asTextureFormats[];
extern const UINT g_uTextureFormatsCount;

extern const D3D7::DDPIXELFORMAT g_pf16_565;
extern const D3D7::DDPIXELFORMAT g_pf16_1555;
extern const D3D7::DDPIXELFORMAT g_pf16_4444;
extern const D3D7::DDPIXELFORMAT g_pf8_Pal;
extern const D3D7::DDPIXELFORMAT g_pf16_v8u8;

extern D3D7::DDPIXELFORMAT g_asZBufferFormats[];
extern const UINT g_uZBufferFormatsCount;

extern const D3D7::DDPIXELFORMAT g_asD2GIPF_To_DD7PF[];
extern const D3D9::D3DFORMAT g_asD2GIPF_To_D3D9PF[];

D2GIPIXELFORMAT DD7PF_To_D2GIPF(D3D7::DDPIXELFORMAT*);
bool IsStdDisplayMode(DWORD dwWidth, DWORD dwHeight);
