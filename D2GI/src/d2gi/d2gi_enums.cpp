
#define INITGUID
#include "d2gi_enums.h"


using namespace D3D7;


const DDPIXELFORMAT g_pf16_565 =
{
	sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16, 63488, 2016, 31, 0,
};


const DDPIXELFORMAT g_pf16_555 =
{
	sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16, 31744, 992, 31, 0,
};


const DDPIXELFORMAT g_pf16_1555 =
{
	sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_ALPHAPIXELS, 0, 16, 31744, 992, 31, 32768
};


const DDPIXELFORMAT g_pf16_4444 =
{
	sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_ALPHAPIXELS, 0, 16, 3840, 240, 15, 61440
};


const DDPIXELFORMAT g_pf16_v8u8 =
{
	sizeof(DDPIXELFORMAT), DDPF_BUMPDUDV, 0, 16, 255, 65280, 0, 0
};


const DDPIXELFORMAT g_pf8_Pal =
{
	sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_PALETTEINDEXED8, 0, 8, 0, 0, 0, 0,
};


const DDPIXELFORMAT g_pf16_depth =
{
	sizeof(DDPIXELFORMAT), DDPF_ZBUFFER, 0, 16, 0, 65535, 0, 0
};


const DDSURFACEDESC2 g_asStdDisplayModes[] =
{
	// 640x480x8
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		480, 640, 640, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf8_Pal
	},
	// 640x480x16
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		480, 640, 1280, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf16_565
	},
	// 800x600x8
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		600, 800, 800, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf8_Pal
	},
	// 800x600x16
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		600, 800, 1600, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf16_565
	},
	// 1024x768x8
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		768, 1024, 1024, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf8_Pal
	},
	// 1024x768x16
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		768, 1024, 2048, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf16_565
	},
	// 1600x1200x8
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		1200, 1600, 1600, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf8_Pal
	},
	// 1600x1200x16
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		1200, 1600, 3200, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf16_565
	},
};

const UINT g_uStdDisplayModesCount = ARRAYSIZE(g_asStdDisplayModes);


const DDCAPS g_sHALCaps = 
{
	380, 2513600449, 2691346992, 189201, 581784800,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1778384896, 1774188544, 1, 0, 18, 0, 0, 0, 0, 0,
	{0, 0, 0, 0, 0, 0, 4096, 0}, {809923324},
	32, 2048000, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0,
	{0, 0, 0, 0, 0, 0, 4096, 0}, 64, 0, 0,
	{0, 0, 0, 0, 0, 0, 4096, 0}, 0, 0, 0,
	{0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 2513600449,
	2690822704, 189201, 581784800,
	{0, 0, 0, 0, 0, 0, 4096, 0}, 809923324, 512, 0, 0,
};


const DDCAPS g_sHELCaps =
{
	380, 4106256961, 1, 512, 261347, 0, 839, 
	0, 0, 0, 0, 0, 0, 0, 1024, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	{1, 0, 0, 0, 0, 0, 4096, 2147483648}, {4330320},
	0, 0, 0, 0, 0, 0, 0, 0, 0, 4106256961, 512,
	261347, {1, 0, 0, 0, 0, 0, 4096, 2147483648},
	4106256961, 512, 261347,
	{1, 0, 0, 0, 0, 0, 4096, 2147483648}, 
	4106256961, 512, 261347,
	{1, 0, 0, 0, 0, 0, 4096, 2147483648},
	0, 0, 0, 4106256961, 1, 512, 261347,
	{1, 0, 0, 0, 0, 0, 4096, 2147483648},
	4330320, 512, 0, 0,
};


D3DDEVICEDESC7 g_asDeviceDescs[] =
{
	{
		1361,
		{
			56, 112, 2171383, 255, 8191, 2047, 255,
			807562, 223, 50529087, 207, 31, 4, 4,
		},
		{
			56, 112, 2171383, 255, 8191, 2047,
			255, 807562, 223, 50529087, 207, 31, 4, 4,
		},
		3840, 1024, 1, 1, 1024, 1024, 256, 0, 1,
		-4096.0, -4096.0, 4095.0, 4095.0, 0.0, 255, 8,
		30975, 8, 8, 4294967295, 10000.0,
		IID_IDirect3DRGBDevice, 6, 4, 63, 0, 0, 0, 0,
	},
	{
		235449937,
		{
			56, 2, 3301809, 255, 8191, 2047, 255,
			807562, 6237, 117638975, 207, 63, 0, 0,
		},
		{
			56, 114, 3301809, 255, 8191, 2047, 255, 
			807562, 6237, 117638975, 207, 63, 0, 0,
		},
		1280, 1536, 1, 1, 8192, 8192, 8192, 8192, 16,
		-16384.0, -16384.0, 16384.0, 16384.0, 0.0,
		255, 524296, 16777215, 4, 4, 4294967295,
		1.0e+10, IID_IDirect3DHALDevice, 6, 4, 63,
		0, 0, 0, 0, 
	},
	{
		235515473,
		{
			56, 2, 3301809, 255, 8191, 2047, 255,
			807562, 6237, 117638975, 207, 63, 0, 0
		},
		{
			56, 114, 3301809, 255, 8191, 2047, 255,
			807562, 6237, 117638975, 207, 63, 0, 0
		},
		1280, 1536, 1, 1, 8192, 8192, 8192, 8192, 16,
		-16384.0, -16384.0, 16384.0, 16384.0, 0.0,
		255, 524296, 16777215, 4, 4, 8, 1.0e+10,
		IID_IDirect3DTnLHalDevice, 6, 4, 379, 0, 0, 0, 0, 
	},
};


CHAR* g_lpszDeviceDescs[] = 
{
	"Microsoft Direct3D RGB Software Emulation",
	"Microsoft Direct3D Hardware acceleration through Direct3D HAL",
	"Microsoft Direct3D Hardware Transform and Lighting acceleration capable device"
};

CHAR* g_lpszDeviceNames[] = { 
	"RGB Emulation",
	"Direct3D HAL",
	"Direct3D T&L HAL" 
};

const UINT g_uDeviceCount = ARRAYSIZE(g_asDeviceDescs);


DDPIXELFORMAT g_asTextureFormats[] =
{
	g_pf16_565,
	g_pf16_555,
	g_pf16_1555,
	g_pf16_4444,
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32, 16711680, 65280, 255, 0},
	{sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_ALPHAPIXELS, 0, 32, 16711680, 65280, 255, 4278190080},
	{sizeof(DDPIXELFORMAT), DDPF_LUMINANCE | DDPF_ALPHAPIXELS, 0, 16, 255, 0, 0, 65280},
	{sizeof(DDPIXELFORMAT), DDPF_LUMINANCE, 0, 8, 255, 0, 0, 0},
	{sizeof(DDPIXELFORMAT), DDPF_LUMINANCE | DDPF_ALPHAPIXELS, 0, 8, 15, 0, 0, 240},
	{sizeof(DDPIXELFORMAT), DDPF_ALPHA, 0, 8, 0, 0, 0, 0},
	g_pf16_v8u8,
	{sizeof(DDPIXELFORMAT), DDPF_BUMPDUDV | DDPF_BUMPLUMINANCE, 0, 16, 31, 992, 64512, 0},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, 844715353},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, 1498831189},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, 1448433985},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, 827611204},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, 844388420},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, 861165636},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, 877942852},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, 894720068},
};
const UINT g_uTextureFormatsCount = ARRAYSIZE(g_asTextureFormats);


DDPIXELFORMAT g_asZBufferFormats[] =
{
	g_pf16_depth,
	{sizeof(DDPIXELFORMAT), DDPF_ZBUFFER, 0, 32, 0, 16777215, 0, 0},
	{sizeof(DDPIXELFORMAT), DDPF_ZBUFFER | DDPF_STENCILBUFFER, 0, 32, 8, 65535, 4278190080, 0},
	{sizeof(DDPIXELFORMAT), DDPF_ZBUFFER, 0, 24, 0, 16777215, 0, 0},
};

const UINT g_uZBufferFormatsCount = ARRAYSIZE(g_asZBufferFormats);


const DDPIXELFORMAT g_asD2GIPF_To_DD7PF[] = 
{
	g_pf8_Pal,
	g_pf16_565,
	g_pf16_1555,
	g_pf16_4444,
	g_pf16_v8u8,
	g_pf16_depth,
	g_pf16_555,
};


const D3D9::D3DFORMAT g_asD2GIPF_To_D3D9PF[] =
{
	D3D9::D3DFMT_P8,
	D3D9::D3DFMT_R5G6B5,
	D3D9::D3DFMT_A1R5G5B5,
	D3D9::D3DFMT_A4R4G4B4,
	D3D9::D3DFMT_V8U8,
	D3D9::D3DFMT_D16,
	D3D9::D3DFMT_A1R5G5B5,
};


D2GIPIXELFORMAT DD7PF_To_D2GIPF(D3D7::DDPIXELFORMAT* ppf)
{
	if (*ppf == g_pf8_Pal)
		return D2GIPF_8_PAL;
	else if (*ppf == g_pf16_565)
		return D2GIPF_16_565;
	else if (*ppf == g_pf16_1555)
		return D2GIPF_16_1555;
	else if (*ppf == g_pf16_4444)
		return D2GIPF_16_4444;
	else if (*ppf == g_pf16_v8u8)
		return D2GIPF_16_V8U8;
	else if (*ppf == g_pf16_depth)
		return D2GIPF_16_DEPTH;
	else if (*ppf == g_pf16_555)
		return D2GIPF_16_555;
	
	return D2GIPF_UNKNOWN;
}


bool IsStdDisplayMode(DWORD dwWidth, DWORD dwHeight)
{
	for (const DDSURFACEDESC2& desc : g_asStdDisplayModes)
	{
		if (desc.dwWidth == dwWidth && desc.dwHeight == dwHeight)
			return TRUE;
	}

	return FALSE;
}
