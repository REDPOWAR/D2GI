#pragma once

#include "common.h"
#include "m3x4.h"


struct D2OBJECT
{
	BYTE data1[1224];
	D3D9::D3DXVECTOR3 avTransformedPortal[4];
	BYTE data2[216];
	DWORD adw1[4];
	BYTE data3[72];
	DWORD adw2[4];
	BYTE data4[72];
	DWORD adw3[4];
	BYTE data5[76];
	DWORD adw4[4];
	BYTE data6[468];
	MAT3X4 amMatrices[3];
	BYTE data7[5840];
	DWORD dwIdx;
};


VOID InjectHooks();
INT CallOriginalCheckRoomPortalVisibility(D2OBJECT* pThis, D3D9::D3DXVECTOR3* pPortal, INT nVal);
