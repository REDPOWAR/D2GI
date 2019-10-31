#pragma once

#include "../d3d7.h"
#include "../d3d9.h"


enum D2GIPIXELFORMAT
{
	D2GIPF_UNKNOWN = -1,
	D2GIPF_8_PAL,
	D2GIPF_16_565,
	D2GIPF_16_1555,
	D2GIPF_16_4444,
	D2GIPF_16_V8U8,
	D2GIPF_16_DEPTH,
};


extern D3D7::DDSURFACEDESC2 g_asAvailableDisplayModes[];
extern UINT g_uAvailableDisplayModesCount;

extern D3D7::DDCAPS g_sHALCaps;
extern D3D7::DDCAPS g_sHELCaps;

extern D3D7::D3DDEVICEDESC7 g_asDeviceDescs[];
extern CHAR* g_lpszDeviceDescs[];
extern CHAR* g_lpszDeviceNames[];
extern UINT g_uDeviceCount;

extern D3D7::DDPIXELFORMAT g_asTextureFormats[];
extern UINT g_uTextureFormatsCount;

extern D3D7::DDPIXELFORMAT g_pf16_565;
extern D3D7::DDPIXELFORMAT g_pf16_1555;
extern D3D7::DDPIXELFORMAT g_pf16_4444;
extern D3D7::DDPIXELFORMAT g_pf8_Pal;
extern D3D7::DDPIXELFORMAT g_pf16_v8u8;

extern D3D7::DDPIXELFORMAT g_asZBufferFormats[];
extern UINT g_uZBufferFormatsCount;

extern D3D7::DDPIXELFORMAT g_asD2GIPF_To_DD7PF[];
extern D3D9::D3DFORMAT g_asD2GIPF_To_D3D9PF[];

D2GIPIXELFORMAT DD7PF_To_D2GIPF(D3D7::DDPIXELFORMAT*);
