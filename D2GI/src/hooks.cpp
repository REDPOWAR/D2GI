
#include "common.h"
#include "hooks.h"

#include "d2gi/d2gi.h"
#include "d2gi/d2gi_device.h"


#define CALL_INSTRUCTION_SIZE 5
#define OPCODE_SIZE           1


HookInjector::D2VERSION HookInjector::s_eCurrentD2Version;


D2GI* HookInjector::ObtainD2GI()
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


INT HookInjector::SetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj)
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
		call HookInjector::SetupTransforms;
		ret 8;
	};
}


INT HookInjector::CallOriginalSetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj)
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


HookInjector::D2VERSION HookInjector::DetectD2Version()
{
	static DWORD c_adwTimestamps[] =
	{
		0x400502EA, 0x4760F7AC
	};

	TCHAR szEXEPath[MAX_PATH];
	FILE* pFile;
	IMAGE_DOS_HEADER sDOSHeader;
	IMAGE_FILE_HEADER sImageHeader;
	INT i;

	GetModuleFileName(NULL, szEXEPath, MAX_PATH);
	pFile = _tfopen(szEXEPath, TEXT("rb"));
	fread(&sDOSHeader, sizeof(sDOSHeader), 1, pFile);
	fseek(pFile, sDOSHeader.e_lfanew + 4, SEEK_SET);
	fread(&sImageHeader, sizeof(sImageHeader), 1, pFile);
	fclose(pFile);

	for (i = 0; i < ARRAYSIZE(c_adwTimestamps); i++)
		if (c_adwTimestamps[i] == sImageHeader.TimeDateStamp)
			return (D2VERSION)i;

	return D2V_UNKNOWN;
}


BOOL HookInjector::PatchCallOperation(DWORD dwOperationAddress, DWORD dwNewCallAddress)
{
	INT nCallOffset;

	nCallOffset = (INT32)dwNewCallAddress;
	nCallOffset -= (INT32)dwOperationAddress + CALL_INSTRUCTION_SIZE;

	return WriteProcessMemory(GetCurrentProcess(), 
		(BYTE*)dwOperationAddress + OPCODE_SIZE, &nCallOffset, sizeof(nCallOffset), NULL);
}


VOID HookInjector::InjectHooks()
{
	DWORD c_adwSetupTransformsCalls[] =
	{
		0x005EB682, 0x005EB622
	};

	s_eCurrentD2Version = DetectD2Version();
	if (s_eCurrentD2Version == D2V_UNKNOWN)
		return;

	PatchCallOperation(c_adwSetupTransformsCalls[s_eCurrentD2Version], (DWORD)SetupTransformsHook);
}
