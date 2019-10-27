#pragma once

#include "d3d7.h"


VOID DebugSurfaceDesc(D3D7::DDSURFACEDESC2* pDesc);
UINT CalcFVFStride(DWORD);
UINT CalcPrimitiveCount(D3D7::D3DPRIMITIVETYPE, DWORD dwVertexCount);
UINT CalcFVFTextureCount(DWORD);
UINT CalcIndexedPrimitiveCount(D3D7::D3DPRIMITIVETYPE, DWORD dwIndexCount);
