#pragma once

#include "d3d7.h"
#include "d3d9.h"


VOID DebugSurfaceDesc(D3D7::DDSURFACEDESC2* pDesc);
UINT CalcFVFStride(DWORD);
UINT CalcPrimitiveCount(D3D7::D3DPRIMITIVETYPE, DWORD dwVertexOrIndexCount);
UINT CalcFVFTextureCount(DWORD);
VOID CalcMipMapLevelSize(DWORD, DWORD, UINT, DWORD*, DWORD*);
VOID ScaleProjectionAspectRatio(D3D9::D3DXMATRIX*, FLOAT fScale);
