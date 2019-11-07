
#include "common.h"
#include "hooks.h"

#include "d2gi/d2gi.h"
#include "d2gi/d2gi_device.h"


#define DEVICE_PTR_ADDRESS   0x00720908
#define CHECK_PORTAL_ADDRESS 0x005B5710

#define OBTAIN_DEVICE()   (*(D3D7::IDirect3DDevice7**)DEVICE_PTR_ADDRESS)

#define CALL_INSTRUCTION_SIZE 5
#define OPCODE_SIZE 1


DWORD g_adwCheckSphereCalls[] =
{
	0x005F1763,
	0x004C558A,
	0x0059A9B3,
	0x005F26CA, 
	0x005F2744,
	0x005F36F4,
	0x005F3019,
	0x005F9941,
	0x005F358E,
	0x005FA9F2,
	0x005F41E1,
	0x005470B4,
	0x005FA7B6,
	0x005FB85F,
	0x0060E765,
	0x0060E899,
};


DWORD g_adwCheckPortalCalls[] =
{
	0x005FB8D4
};


UINT32 __stdcall CheckSphereVisibility(VOID* pThis, SPHERE* pSphere)
{
	D3D7::IDirect3DDevice7* pDev;

	pDev = *(D3D7::IDirect3DDevice7**)DEVICE_PTR_ADDRESS;
	if (pDev != NULL)
		return ((D2GIDevice*)pDev)->GetD2GI()->OnSphereVisibilityCheck(pThis, pSphere);

	return 0;
}

__declspec(naked) VOID CheckSphereVisibilityHook()
{
	__asm
	{
		mov eax, [esp + 4];
		push eax;
		push ecx;
		call CheckSphereVisibility;
		ret 4;
	};
}


INT CallOriginalCheckRoomPortalVisibility(D2OBJECT* pThis, D3D9::D3DXVECTOR3* pPortal, INT nVal)
{
	INT nResult, nAddr = CHECK_PORTAL_ADDRESS;

	__asm
	{
		push ecx;
		push eax;

		mov ecx, pThis;
		push nVal;
		push pPortal;
		call nAddr;

		mov nResult, eax;
		pop eax;
		pop ecx;
	};

	return nResult;
}


INT __stdcall CheckRoomPortalVisibility(D2OBJECT* pThis, D3D9::D3DXVECTOR3* pPortal, INT nVal)
{
	D3D7::IDirect3DDevice7* pDev = OBTAIN_DEVICE();

	if (pDev == NULL)
		return CallOriginalCheckRoomPortalVisibility(pThis, pPortal, nVal);

	return ((D2GIDevice*)pDev)->GetD2GI()->OnRoomPortalVisibilityCheck(pThis, pPortal, nVal);
}


__declspec(naked) VOID CheckRoomPortalVisibilityHook()
{
	__asm
	{
		mov eax, [esp + 8];
		push eax;
		mov eax, [esp + 8];
		push eax;
		push ecx;
		call CheckRoomPortalVisibility
		ret 8;
	}
}


VOID InjectHooks()
{
	INT32 nCallOffset;
	DWORD dwCurrentAddress;
	SIZE_T uBytesWritten;
	INT i;

	for (i = 0; i < ARRAYSIZE(g_adwCheckSphereCalls); i++)
	{
		dwCurrentAddress = g_adwCheckSphereCalls[i];

		nCallOffset = (INT32)CheckSphereVisibilityHook;
		nCallOffset -= (INT32)dwCurrentAddress + CALL_INSTRUCTION_SIZE;

		if (!WriteProcessMemory(GetCurrentProcess(), (BYTE*)dwCurrentAddress + OPCODE_SIZE,
			&nCallOffset, sizeof(nCallOffset), &uBytesWritten))
			uBytesWritten = uBytesWritten;
	}


	for (i = 0; i < ARRAYSIZE(g_adwCheckPortalCalls); i++)
	{
		dwCurrentAddress = g_adwCheckPortalCalls[i];

		nCallOffset = (INT32)CheckRoomPortalVisibilityHook;
		nCallOffset -= (INT32)dwCurrentAddress + CALL_INSTRUCTION_SIZE;

		if (!WriteProcessMemory(GetCurrentProcess(), (BYTE*)dwCurrentAddress + OPCODE_SIZE,
			&nCallOffset, sizeof(nCallOffset), &uBytesWritten))
			uBytesWritten = uBytesWritten;
	}
}
