#pragma once

#include "common.h"
#include "m3x4.h"


class D2GI;


class HookInjector
{
	enum D2VERSION
	{
		D2V_UNKNOWN = -1,
		D2V_8_1,
		D2V_8_1B,
	};

	static D2VERSION s_eCurrentD2Version;

	static D2GI* ObtainD2GI();
	static INT __stdcall SetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj);
	static INT CallOriginalSetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj);

	static D2VERSION DetectD2Version();
	static BOOL PatchCallOperation(DWORD dwOperationAddress, DWORD dwNewCallAddress);
public:
	static VOID InjectHooks();
};
