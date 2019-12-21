
#include "../common/common.h"
#include "../common/logger.h"
#include "../common/dir.h"

#include "d2gi.h"
#include "d2gi_device.h"
#include "d2gi_hooks.h"
#include "d2gi_config.h"


#define CALL_INSTRUCTION_SIZE 5
#define OPCODE_SIZE           1


D2GIHookInjector::D2VERSION D2GIHookInjector::s_eCurrentD2Version;


D2GI* D2GIHookInjector::ObtainD2GI()
{
	static DWORD c_adwDeviceAddresses[] =
	{
		0x00720908, 0x00720928
	};

	DWORD                   dwDevAddr = c_adwDeviceAddresses[s_eCurrentD2Version];
	D3D7::IDirect3DDevice7* pDev      = *(D3D7::IDirect3DDevice7**)dwDevAddr;

	if (pDev == NULL)
		return NULL;

	return ((D2GIDevice*)pDev)->GetD2GI();
}


INT D2GIHookInjector::SetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj)
{
	D2GI* pD2GI = ObtainD2GI();
	MAT3X4 mPatchedView = *pmView, mPatchedProj = *pmProj;

	if(pD2GI != NULL)
		pD2GI->OnTransformsSetup(pThis, &mPatchedView, &mPatchedProj);

	return CallOriginalSetupTransforms(pThis, &mPatchedView, &mPatchedProj);
}


__declspec(naked) VOID SetupTransformsHook()
{
	__asm
	{
		mov eax, [esp + 8];
		push eax;
		mov eax, [esp + 8];
		push eax;
		push ecx;
		call D2GIHookInjector::SetupTransforms;
		ret 8;
	};
}


INT D2GIHookInjector::CallOriginalSetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj)
{
	static DWORD c_adwSetupTransformsAddresses[] =
	{
		0x005AE0E0, 0x005AE070
	};

	INT nResult, nAddr = c_adwSetupTransformsAddresses[s_eCurrentD2Version];

	__asm
	{
		push ecx;
		push eax;

		mov ecx, pThis;
		push pmProj;
		push pmView;
		call nAddr;

		mov nResult, eax;
		pop eax;
		pop ecx;
	};

	return nResult;
}


D2GIHookInjector::D2VERSION D2GIHookInjector::DetectD2Version()
{
	static DWORD c_adwTimestamps[] =
	{
		0x400502EA, 0x4760F7AC
	};

	FILE* pFile;
	IMAGE_DOS_HEADER sDOSHeader;
	IMAGE_FILE_HEADER sImageHeader;
	INT i;

	pFile = _tfopen(Directory::GetEXEPath(), TEXT("rb"));
	if (pFile == NULL)
	{
		Logger::Warning(
			TEXT("Failed to open D2 EXE file to detect version (%s)", Directory::GetEXEPath()));
		return D2V_UNKNOWN;
	}

	fread(&sDOSHeader, sizeof(sDOSHeader), 1, pFile);
	fseek(pFile, sDOSHeader.e_lfanew + 4, SEEK_SET);
	fread(&sImageHeader, sizeof(sImageHeader), 1, pFile);
	fclose(pFile);

	for (i = 0; i < ARRAYSIZE(c_adwTimestamps); i++)
		if (c_adwTimestamps[i] == sImageHeader.TimeDateStamp)
			return (D2VERSION)i;

	return D2V_UNKNOWN;
}


BOOL D2GIHookInjector::PatchCallOperation(DWORD dwOperationAddress, DWORD dwNewCallAddress)
{
	INT nCallOffset;

	nCallOffset = (INT32)dwNewCallAddress;
	nCallOffset -= (INT32)dwOperationAddress + CALL_INSTRUCTION_SIZE;

	return WriteProcessMemory(GetCurrentProcess(), 
		(BYTE*)dwOperationAddress + OPCODE_SIZE, &nCallOffset, sizeof(nCallOffset), NULL);
}


VOID D2GIHookInjector::InjectHooks()
{
	static DWORD c_adwSetupTransformsCalls[] =
	{
		0x005EB682, 0x005EB622
	};
	static TCHAR* c_lpszVersionNames[] =
	{
		TEXT("8.1"),
		TEXT("8.1B")
	};


	if (!D2GIConfig::HooksEnabled())
	{
		Logger::Log(TEXT("Hook injection is not enabled."));
		return;
	}

	Logger::Log(TEXT("Trying to inject hooks..."));

	s_eCurrentD2Version = DetectD2Version();
	if (s_eCurrentD2Version == D2V_UNKNOWN)
	{
		Logger::Log(TEXT("Current D2 executable version is unknown, injection aborted"));
		return;
	}

	Logger::Log(TEXT("Detected D2 version %s"), c_lpszVersionNames[s_eCurrentD2Version]);
	if (PatchCallOperation(c_adwSetupTransformsCalls[s_eCurrentD2Version], (DWORD)SetupTransformsHook))
		Logger::Log(TEXT("Successfully injected hooks"));
	else
		Logger::Log(TEXT("Unable to write process memory to inject hooks"));
}
