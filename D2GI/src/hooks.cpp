
#include "common.h"
#include "hooks.h"

#include "d2gi/d2gi.h"
#include "d2gi/d2gi_device.h"


#define DEVICE_PTR_ADDRESS       0x00720908
#define SETUP_TRANSFORMS_ADDRESS 0x005AE0E0

#define OBTAIN_DEVICE()   (*(D3D7::IDirect3DDevice7**)DEVICE_PTR_ADDRESS)

#define CALL_INSTRUCTION_SIZE 5
#define OPCODE_SIZE 1


DWORD g_adwSetupTransformsCalls[] =
{
	0x005EB682,
};


INT CallOriginalSetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj)
{
	INT nResult, nAddr = SETUP_TRANSFORMS_ADDRESS;

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


INT __stdcall SetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj)
{
	D3D7::IDirect3DDevice7* pDev = OBTAIN_DEVICE();

	if (pDev == NULL)
		return CallOriginalSetupTransforms(pThis, pmView, pmProj);

	return ((D2GIDevice*)pDev)->GetD2GI()->OnTransformsSetup(pThis, pmView, pmProj);
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
		call SetupTransforms;
		ret 8;
	};
}



VOID InjectHooks()
{
	INT32 nCallOffset;
	DWORD dwCurrentAddress;
	SIZE_T uBytesWritten;
	INT i;

	for (i = 0; i < ARRAYSIZE(g_adwSetupTransformsCalls); i++)
	{
		dwCurrentAddress = g_adwSetupTransformsCalls[i];

		nCallOffset = (INT32)SetupTransformsHook;
		nCallOffset -= (INT32)dwCurrentAddress + CALL_INSTRUCTION_SIZE;

		if (!WriteProcessMemory(GetCurrentProcess(), (BYTE*)dwCurrentAddress + OPCODE_SIZE,
			&nCallOffset, sizeof(nCallOffset), &uBytesWritten))
			uBytesWritten = uBytesWritten;
	}
}
