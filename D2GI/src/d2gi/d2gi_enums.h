#pragma once

#include "../d3d7.h"


extern D3D7::DDSURFACEDESC2 g_asAvailableDisplayModes[];
extern UINT g_uAvailableDisplayModesCount;

extern D3D7::DDCAPS g_sHALCaps;
extern D3D7::DDCAPS g_sHELCaps;

extern D3D7::D3DDEVICEDESC7 g_asDeviceDescs[];
extern CHAR* g_lpszDeviceDescs[];
extern CHAR* g_lpszDeviceNames[];
extern UINT g_uDeviceCount;