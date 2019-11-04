
#include "common.h"

#include "d2gi/d2gi.h"
#include "d2gi/d2gi_device.h"


#define DEVICE_PTR_ADDRESS 0x00720908

#define CALL_INSTRUCTION_SIZE 5
#define OPCODE_SIZE 1


DWORD g_adwCallInstructionAddresses[] =
{
	0x005F1763,
};


DWORD __stdcall CheckSphereVisibilityHook(SPHERE* pSphere)
{
	VOID* pThis;
	D2GI* pD2GI;

	__asm
	{
		mov pThis, ecx;
	};

	pD2GI = ((D2GIDevice*)(D3D7::IDirect3DDevice7*)DEVICE_PTR_ADDRESS)->GetD2GI();

	return pD2GI->OnSphereVisibilityCheck(pThis, pSphere);
}



VOID InjectHooks()
{
	INT i;

	for (i = 0; i < ARRAYSIZE(g_adwCallInstructionAddresses); i++)
	{
		INT32 nCallOffset;
		DWORD dwCurrentAddress = g_adwCallInstructionAddresses[i];
		SIZE_T uBytesWritten;

		nCallOffset = (INT32)CheckSphereVisibilityHook;
		nCallOffset -= (INT32)g_adwCallInstructionAddresses[i] + CALL_INSTRUCTION_SIZE; 

		WriteProcessMemory(GetCurrentProcess(), (BYTE*)dwCurrentAddress + OPCODE_SIZE,
			&nCallOffset, sizeof(nCallOffset), &uBytesWritten);
	}
}
