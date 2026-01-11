#pragma once

#include "../common/common.h"
#include "../common/m3x4.h"

#include "d2gi_common.h"


class D2GIHookInjector
{
	enum D2VERSION
	{
		D2V_5_5 = 0,
		D2V_8_1,
		D2V_8_1B,
		D2V_KOTR_1_3,

		D2V_UNKNOWN,
		NUM_D2VERSIONS = D2V_UNKNOWN,
	};

	static D2GI* ObtainD2GI();
	static void __fastcall SetupTransforms(void* pThis, void*, MAT3X4* pmView, MAT3X4* pmProj);

	static D2VERSION DetectD2Version();
public:
	static void InjectHooks();

private:
	static void (__thiscall *m_origSetupTransform)(void* pThis, MAT3X4* pmView, MAT3X4* pmProj);
	static D3D7::IDirect3DDevice7** m_deviceAddress;
};
