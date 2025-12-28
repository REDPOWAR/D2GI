
#include "../common/common.h"
#include "../common/logger.h"

#include "d2gi.h"
#include "d2gi_device.h"
#include "d2gi_hooks.h"
#include "d2gi_config.h"


#define CALL_INSTRUCTION_SIZE 5
#define OPCODE_SIZE           1


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

	const HMODULE gameModule = GetModuleHandle(nullptr);

	PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(gameModule);
	PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<char*>(dosHeader) + dosHeader->e_lfanew);

	for (size_t i = 0; i < ARRAYSIZE(c_adwTimestamps); i++)
		if (c_adwTimestamps[i] == ntHeader->FileHeader.TimeDateStamp)
			return (D2VERSION)i;

	return D2V_UNKNOWN;
}


void D2GIHookInjector::PatchCallOperation(DWORD_PTR dwOperationAddress)
{
	DWORD dwProtect;
	VirtualProtect((void*)dwOperationAddress, CALL_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &dwProtect);

	INT nOriginalCallOffset;
	memcpy(&nOriginalCallOffset, (void*)(dwOperationAddress + OPCODE_SIZE), sizeof(nOriginalCallOffset));
	m_origSetupTransform = (decltype(m_origSetupTransform))(nOriginalCallOffset + dwOperationAddress + CALL_INSTRUCTION_SIZE);

	const INT nCallOffset = (INT32)&SetupTransforms - (INT32)dwOperationAddress - CALL_INSTRUCTION_SIZE;
	memcpy((void*)(dwOperationAddress + OPCODE_SIZE), &nCallOffset, sizeof(nCallOffset));

	VirtualProtect((void*)dwOperationAddress, CALL_INSTRUCTION_SIZE, dwProtect, &dwProtect);
}


void D2GIHookInjector::InjectHooks()
{
	const DWORD c_adwSetupTransformsCalls[] =
	{
		0x005EB682, 0x005EB622, 0x005EACB2
	};
	const DWORD c_adwDeviceAddresses[] =
	{
		0x00720908, 0x00720928, 0x0071F868
	};
	const TCHAR* c_lpszVersionNames[] =
	{
		TEXT("8.1"),
		TEXT("8.1B"),
		TEXT("KoTR 1.3"),
	};


	if (!D2GIConfig::HooksEnabled())
	{
		Logger::Log(TEXT("Hook injection is not enabled."));
		return;
	}

	Logger::Log(TEXT("Trying to inject hooks..."));

	const D2VERSION eCurrentD2Version = DetectD2Version();
	if (eCurrentD2Version == D2V_UNKNOWN)
	{
		Logger::Log(TEXT("Current D2 executable version is unknown, injection aborted"));
		return;
	}

	Logger::Log(TEXT("Detected D2 version %s"), c_lpszVersionNames[eCurrentD2Version]);
	PatchCallOperation(c_adwSetupTransformsCalls[eCurrentD2Version]);
	m_deviceAddress = (D3D7::IDirect3DDevice7**)c_adwDeviceAddresses[eCurrentD2Version];
}
