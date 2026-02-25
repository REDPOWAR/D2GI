#pragma once

#include "../common/common.h"
#include "../common/m3x4.h"

#include "d2gi_common.h"

struct HookOptions;

class D2GIHookInjector
{
	static void __fastcall SetupTransforms(void* pThis, void*, MAT3X4* pmView, MAT3X4* pmProj);

	static int GetGameVersion();
	static void __cdecl WriteScreenshotFunc(void* a2);
	static void InjectScreenshotsPatch();

	static void OnPrepareStartGame(void* a1, void* a2);
	static void OnInitClusters();
	static void InjectMirrorsPatch();
	static void InjectInterfacePatch(bool use_mirrors_fix);
	static signed int __fastcall OnInitDrawForGame(int* CWinApp, int EDX, int width, int height, int depth, int a5);

public:
	static D2GI* ObtainD2GI();
	static void InjectHooks(const HookOptions& options);

private:
	static void (__thiscall *m_origSetupTransform)(void* pThis, MAT3X4* pmView, MAT3X4* pmProj);
	static D3D7::IDirect3DDevice7** m_deviceAddress;
};
