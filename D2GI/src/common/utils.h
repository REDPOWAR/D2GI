#pragma once

#include "d3d7.h"
#include "d3d9.h"


size_t CalcFVFStride(DWORD dwFVF);
size_t CalcOffsetToPSize(DWORD dwFVF);
UINT CalcPrimitiveCount(D3D7::D3DPRIMITIVETYPE, DWORD dwVertexOrIndexCount);
size_t CalcFVFTextureCount(DWORD);
VOID CalcMipMapLevelSize(DWORD, DWORD, UINT, DWORD*, DWORD*);
