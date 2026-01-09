
#include "../common/common.h"
#include "../common/logger.h"

#include "d2gi.h"
#include "d2gi_device.h"
#include "d2gi_hooks.h"
#include "d2gi_config.h"

#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"


void (__thiscall *D2GIHookInjector::m_origSetupTransform)(void* pThis, MAT3X4* pmView, MAT3X4* pmProj);
D3D7::IDirect3DDevice7** D2GIHookInjector::m_deviceAddress;


D2GI* D2GIHookInjector::ObtainD2GI()
{
	D3D7::IDirect3DDevice7* pDev      = *m_deviceAddress;
	if (pDev == NULL)
		return NULL;

	return ((D2GIDevice*)pDev)->GetD2GI();
}


void __fastcall D2GIHookInjector::SetupTransforms(void* pThis, void*, MAT3X4* pmView, MAT3X4* pmProj)
{
	D2GI* pD2GI = ObtainD2GI();
	MAT3X4 mPatchedView = *pmView, mPatchedProj = *pmProj;

	if(pD2GI != NULL)
		pD2GI->OnTransformsSetup(pThis, &mPatchedView, &mPatchedProj);

	m_origSetupTransform(pThis, &mPatchedView, &mPatchedProj);
}


D2GIHookInjector::D2VERSION D2GIHookInjector::DetectD2Version()
{
	const DWORD c_adwTimestamps[] =
	{
		0x400502EA, // 8.1
		0x4760F7AC, // 8.1B
		0x3C970FF7, // KotR 1.3
	};
	static_assert(std::size(c_adwTimestamps) == NUM_D2VERSIONS);

	const HMODULE gameModule = GetModuleHandle(nullptr);

	PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(gameModule);
	PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<char*>(dosHeader) + dosHeader->e_lfanew);

	for (size_t i = 0; i < ARRAYSIZE(c_adwTimestamps); i++)
		if (c_adwTimestamps[i] == ntHeader->FileHeader.TimeDateStamp)
			return (D2VERSION)i;

	return D2V_UNKNOWN;
}


void D2GIHookInjector::InjectHooks()
{
	const TCHAR* c_lpszVersionNames[] =
	{
		TEXT("8.1"),
		TEXT("8.1B"),
		TEXT("KoTR 1.3"),

		TEXT("Unknown")
	};
	static_assert(std::size(c_lpszVersionNames) == NUM_D2VERSIONS + 1);
	Logger::Log(TEXT("Detected D2 version: %s"), c_lpszVersionNames[DetectD2Version()]);

	if (!D2GIConfig::HooksEnabled())
	{
		Logger::Log(TEXT("Hook injection is not enabled."));
		return;
	}

	using namespace Memory::VP;
	using namespace hook::txn;

	try
	{
		auto device_address_ptr = get_pattern<D3D7::IDirect3DDevice7**>("8B 0D ? ? ? ? 8D 54 24 ? 51 6A ? 52 68 ? ? ? ? 68 ? ? ? ? C7 44 24", 2);
		auto setup_transforms = get_pattern("50 51 8B CE E8 ? ? ? ? 5F 5E 5D", 4);

		m_deviceAddress = *device_address_ptr;
		InterceptCall(setup_transforms, m_origSetupTransform, &SetupTransforms);
	}
	catch (const hook::txn_exception&)
	{
		Logger::Log(TEXT("Failed to inject hooks, signature scan(s) failed."));
		return;
	}

	Logger::Log(TEXT("Hooks injected."));
}
