
#include "../common/common.h"
#include "../common/logger.h"

#include "d2gi.h"
#include "d2gi_device.h"
#include "d2gi_hooks.h"
#include "d2gi_config.h"

#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"


#include "Utils/CPatch.h"

// Normally a bad practice, but wincodec.h expects some D3D9 types in the global scope, so provide them.
using namespace D3D9;

#include "ScreenGrab/ScreenGrab9.h"
#include <wincodec.h>

#include <wrl/client.h>


void (__thiscall *D2GIHookInjector::m_origSetupTransform)(void* pThis, MAT3X4* pmView, MAT3X4* pmProj);
D3D7::IDirect3DDevice7** D2GIHookInjector::m_deviceAddress;

D2GIHookInjector::D2VERSION D2GIHookInjector::s_eCurrentD2Version;


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
		0x39DC4F94, //v 5.5 - 05.10.2000 [EN]
		0x3C970FF7, //v 1.3 - 19.03.2002 [EN/GOG]
      //0x3E3E392B, //v 8.0 - 03.02.2003 [RU]
		0x400502EA, //v 8.1 - 14.01.2004 [RU/GOG]
		0x4760F7AC, //v 8.2 - 13.12.2007 [RU]
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

//***** D2GI_v2 hooks *****//

class CCoInitialize
{
public:
	CCoInitialize(DWORD dwCoInit) : m_hr(CoInitializeEx(NULL, dwCoInit)) {}
	~CCoInitialize() { if (SUCCEEDED(m_hr)) CoUninitialize(); }
	operator HRESULT() const { return m_hr; }

private:
	const HRESULT m_hr;
};

//struct tagBITMAPINFO *__cdecl WritePhotoToFile(FILE *a2) (0x5E9EA0)
void __cdecl D2GIHookInjector::WriteScreenshotFunc(void *a2)
{
	D2GI* pD2GI = ObtainD2GI();

	if (pD2GI == NULL) {
		Logger::Warning(TEXT("ScreenshotHook->!pD2GI"));
		return;
	}

	CCoInitialize coInit(COINIT_MULTITHREADED);
	if (FAILED(coInit) && coInit != RPC_E_CHANGED_MODE)
	{
		return;
	}

	using namespace Microsoft::WRL;

	ComPtr<D3D9::IDirect3DDevice9> device(pD2GI->GetD3D9Device());
	
	ComPtr<D3D9::IDirect3DSurface9> backbuffer;
	if (FAILED(device->GetRenderTarget(0, backbuffer.GetAddressOf())))
	{
		return;
	}

	D3D9::D3DSURFACE_DESC desc;
	if (FAILED(backbuffer->GetDesc(&desc)))
	{
		return;
	}

	ComPtr<D3D9::IDirect3DSurface9> buffer;
	if (FAILED(device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3D9::D3DPOOL_SYSTEMMEM, buffer.GetAddressOf(), nullptr)))
	{
		return;
	}
	
	if (FAILED(device->GetRenderTargetData(backbuffer.Get(), buffer.Get())))
	{
		return;
	}

	CreateDirectoryW(D2GIConfig::GetScreenshotsPath(), nullptr);

	const wchar_t* extension;
	const GUID* imageContainerFormat;
	switch (D2GIConfig::GetScreenshotsFormat())
	{
	case IMG_PNG:
		extension = L"png";
		imageContainerFormat = &GUID_ContainerFormatPng;
		break;
	case IMG_JPG:
		extension = L"jpg";
		imageContainerFormat = &GUID_ContainerFormatJpeg;
		break;
	case IMG_BMP:
	default:
		extension = L"bmp";
		imageContainerFormat = &GUID_ContainerFormatBmp;
		break;
	}

	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	wchar_t screenshot_path[MAX_PATH];
	swprintf_s(screenshot_path, L"%ls\\ddphoto_%u-%02u-%02u_%02u-%02u-%02u.%ls", D2GIConfig::GetScreenshotsPath(),
		systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond, extension);

	if (SUCCEEDED(DirectX::SaveWICTextureToFile(buffer.Get(), *imageContainerFormat, screenshot_path)))
	{
		Logger::Log(TEXT("Screenshot saved."));
	}
}

void D2GIHookInjector::InjectScreenshotsPatch() {
	//						     5.5,      1.3,      8.1,      8.2
	DWORD scr_WriteAddr[]  = { 0x000000, 0x575B92, 0x5764C2, 0x576452 };
	DWORD scr_mkdirAddr[]  = { 0x000000, 0x575B5E, 0x57648E, 0x57641E };
	DWORD scr_fopenAddr[]  = { 0x000000, 0x575B8A, 0x5764BA, 0x57644A };
	DWORD scr_fcloseAddr[] = { 0x000000, 0x575B98, 0x5764C8, 0x576458 };

	CPatch::RedirectCall(scr_WriteAddr[s_eCurrentD2Version], &WriteScreenshotFunc);

	//_mkdir(".\\screenshots");
	CPatch::Nop(scr_mkdirAddr[s_eCurrentD2Version], 5);

	//v3 = fopen(&Dest, "wb");
	CPatch::Nop(scr_fopenAddr[s_eCurrentD2Version], 5);

	//fclose(v3);
	CPatch::Nop(scr_fcloseAddr[s_eCurrentD2Version], 5);

	Logger::Log(TEXT("Injected screenshots hook."));
}


/////* interface hooks variables */////

//selected item ID in mainMenu->graphics->screenResolution
int m_dwMenuSettingsValue;
//interface aspect (for interiors FOV fix)
float m_fUIAspect;
//resolution forced by D2GI
int m_dwResX, m_dwResY;

void D2GIHookInjector::OnPrepareStartGame() {
	//						     5.5,      1.3,      8.1,      8.2
	DWORD dw_FuncPointer[] = { 0x000000, 0x5126D0, 0x512EA0, 0x512E00 };
	DWORD dw_TheGamePtr [] = { 0x000000, 0x695C00, 0x696CA0, 0x696CC0 };
	DWORD dw_MenuItemID [] = { 0x000000, 0x6CDBD0, 0x6CEC70, 0x6CEC90 };

	//MenuVideo->overwrite selected resolution item ID
	*(int*)(*(int*)dw_MenuItemID[s_eCurrentD2Version] + 408) = m_dwMenuSettingsValue;

	//run original function
	signed int* (*PrepareStartGame)(int*) = (signed int* (*)(int*))dw_FuncPointer[s_eCurrentD2Version];
	PrepareStartGame((int*)dw_TheGamePtr[s_eCurrentD2Version]);
}

void D2GIHookInjector::OnSetupUIOffsets(){
	//						       5.5,      1.3,      8.1,      8.2
	DWORD dw_SetupOffsets [] = { 0x000000, 0x510750, 0x510F20, 0x510E80 };
	//Just two int32 values
	DWORD dw_MenuBackInfoX[] = { 0x000000, 0x6CDC7C, 0x6CED1C, 0x6CED3C };
	DWORD dw_MainSideBarX [] = { 0x000000, 0x6CDC74, 0x6CED14, 0x6CED34 };
	//Pointers to menu object instances
	DWORD dw_MainFuelGasX [] = { 0x000000, 0x6CDC0C, 0x6CECAC, 0x6CECCC };
	DWORD dw_MainNetworkX [] = { 0x000000, 0x6CDC14, 0x6CECB4, 0x6CECD4 };

	//Some offsets for 7.3:
	//Multiplayer menu offset X - *(int*)0x6CC9A0 + 0x38) + 0x28
	//NOT 0x38 + 0x2C, as it was in 8.x/1.3 versions.

	//call original function before changing offsets
	((void(*)(int))dw_SetupOffsets[s_eCurrentD2Version])(0);

	//fix sidebar positions
	*(int*)dw_MenuBackInfoX[s_eCurrentD2Version] = m_dwResX - 385;
	*(int*)dw_MainSideBarX[s_eCurrentD2Version] = m_dwResX - 225;

	*(int*)(*(int*)(*(int*)dw_MainFuelGasX[s_eCurrentD2Version] + 0x38) + 0x2C) = (m_dwResX - 1024) / 2;
	*(int*)(*(int*)(*(int*)dw_MainNetworkX[s_eCurrentD2Version] + 0x38) + 0x2C) = (m_dwResX - 1024) / 2;

}

void D2GIHookInjector::OnInitClusters() {
	//						       5.5,      1.3,      8.1,      8.2
	DWORD dw_InitClusters[] = { 0x000000, 0x52A470, 0x52AD30, 0x52ACB0 };
	DWORD dw_FOV         [] = { 0x000000, 0x695C0C, 0x696CAC, 0x696CCC };

	//This offsets (0x58, 0x54) are tested only at 1.3, 8.1 and 8.2

	//blockObserver->FOV
	*(float*)(*(DWORD*)dw_FOV[s_eCurrentD2Version] + 0x58) = m_fUIAspect * 1.2f;
	*(float*)(*(DWORD*)dw_FOV[s_eCurrentD2Version] + 0x54) = 1.2f;

	//call original function
	void(*InitClusters)() = (void(*)())dw_InitClusters[s_eCurrentD2Version];
	InitClusters();
}

//Interface main injection code
void D2GIHookInjector::InjectInterfacePatch() {
	// How it works:
	// 1. The screen resolution set by D2GI is checked; if its width is higher than 1600, the width
	// becomes equal to 1600, the height is proportionally reduced.
	// 
	// 
	// 2. Next comes the check: if the screen resolution is 4:3, then the interface size will be
	// either 1024x768 or 1600x1200.
	//  If the format resolution is 16:9 or higher, then the interface resolution will be either
	// 1280x720 or 1600x900. If it is 1280x720, then new offsets of the headup display, pager and
	// the general offset of the interface labels on the right side of the screen are written.
	//
	//
	// 3. Next, the resolution values ​​of 1024x768 are overwritten to the new one. All this code
	// is made as a redesign of the 1024x768 resolution interface layout.
	//
	//
	// 4. Installation of hooks:
	// * OnPrepareStartGame - here there is a forced selection of the "1024x768" button in the graphics
	// settings menu of the main game menu;
	// 
	// * OnSetupUIOffsets   - here new offsets of the interface on the right side of the screen are set;
	// 
	// * OnInitClusters     - a new interior FOV is being installed here, because the standard D2GI fix
	// not actual due to interface hooks.

	m_dwResX = D2GIConfig::GetVideoWidth();
	m_dwResY = D2GIConfig::GetVideoHeight();

	float real_aspect = (float)m_dwResX / (float)m_dwResY;
	float aspect_rev = (float)m_dwResY / (float)m_dwResX;

	//1) Clamp resolution to 1600; max in-game GUI textures size is 1600x1200, in other cases
	//textures in esc16.res should be redrawn for higher resolutions.
	if (m_dwResX > 1600) {
		m_dwResX = 1600;
		m_dwResY = (int)((float)m_dwResX / real_aspect);
	}

	//resoultion should be 4:3 or 16:9, otherwise game will crash with "not enough texture memory error"
	
	//possible values:
	// hex   int 
	//0x110 (274) 640x480
	//0x120 (290) 800x600
	//0x130 (306) 1024x768
	//0x140 (322) 1600x900

	m_dwMenuSettingsValue = 306;

	//						   5.5,      1.3,      8.1,      8.2
	DWORD cmp1204addr[] = { 0x000000, 0x5691ED, 0x569ADD, 0x569A6D };
	DWORD cmp800addr [] = { 0x000000, 0x569223, 0x569B13, 0x569AA3 };
	DWORD pagerXaddr [] = { 0x000000, 0x56924B, 0x569B3B, 0x569ACB };
	DWORD pagerYaddr [] = { 0x000000, 0x5691E8, 0x569AD8, 0x569A68 };
	DWORD panelXaddr [] = { 0x000000, 0x569259, 0x569B49, 0x569AD9 };
	DWORD textXaddr  [] = { 0x000000, 0x569254, 0x569B44, 0x569AD4 };

	DWORD m_dwResXAddr[] = { 0x000000, 0x5127F1, 0x512FC1, 0x512F21 };
	DWORD m_dwResYAddr[] = { 0x000000, 0x5127EC, 0x512FBC, 0x512F1C };

	//function call pointers
	DWORD call_prepareGameAddr [] = { 0x000000, 0x510516, 0x510CE6, 0x510C46 };
	DWORD call_setOffsetsAddr  [] = { 0x000000, 0x510732, 0x510F02, 0x510E62 };
	DWORD call_initClustersAddr[] = { 0x000000, 0x4E0505, 0x4E05A5, 0x4E0625 };

	//2) Set resolution from aspect
	if (aspect_rev > 0.7) {
		if (m_dwResX > 1024) {
			m_dwResX = 1600;
			m_dwResY = 1200;
			m_dwMenuSettingsValue = 322;
		}
		else {
			m_dwResX = 1024;
			m_dwResY = 768;
		}
	} else {
		if (m_dwResX > 1280) {
			m_dwResX = 1600;
			m_dwResY = 900;
		}
		else {
			m_dwResX = 1280;
			m_dwResY = 720;

			CPatch::SetInt(cmp1204addr[s_eCurrentD2Version], 1285);
			CPatch::SetInt(cmp800addr[s_eCurrentD2Version],  1284);
			CPatch::SetInt(pagerXaddr[s_eCurrentD2Version],   936);
			CPatch::SetInt(pagerYaddr[s_eCurrentD2Version],    11);
			CPatch::SetInt(panelXaddr[s_eCurrentD2Version],  -225);
			CPatch::SetInt(textXaddr[s_eCurrentD2Version],    244);
		}
	}

	m_fUIAspect = (float)m_dwResX / m_dwResY;

	Logger::Log(TEXT("Current GUI res is %dx%d"), m_dwResX, m_dwResY);

	//3) replace default 1024x768 resolution to new
	CPatch::SetShort(m_dwResXAddr[s_eCurrentD2Version], m_dwResX);
	CPatch::SetShort(m_dwResYAddr[s_eCurrentD2Version], m_dwResY);

	//4) hook functions
	CPatch::RedirectCall(call_prepareGameAddr[s_eCurrentD2Version],  &OnPrepareStartGame);
	CPatch::RedirectCall(call_setOffsetsAddr[s_eCurrentD2Version],   &OnSetupUIOffsets);
	CPatch::RedirectCall(call_initClustersAddr[s_eCurrentD2Version], &OnInitClusters);

	Logger::Log(TEXT("Successfully injected interface hooks"));
}


void D2GIHookInjector::InjectHooks()
{
	const TCHAR* c_lpszVersionNames[] =
	{
		TEXT("5.5"),
		TEXT("1.3"),
		TEXT("8.1"),
		TEXT("8.2"),

		TEXT("Unknown")
	};

	s_eCurrentD2Version = DetectD2Version();

	static_assert(std::size(c_lpszVersionNames) == NUM_D2VERSIONS + 1);
	Logger::Log(TEXT("Detected D2 version: %s"), c_lpszVersionNames[s_eCurrentD2Version]);

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

	Logger::Log(TEXT("Injected common hooks."));

	if (s_eCurrentD2Version != D2V_5_5 && s_eCurrentD2Version != D2V_UNKNOWN) {
		//hooks ONLY for v1.3, v8.1 and v8.2

		//Screenshot save patch
		D2GIHookInjector::InjectScreenshotsPatch();

		//Interface aspect fix
		D2GIHookInjector::InjectInterfacePatch();

	} else {
		Logger::Log(TEXT("Interface and screenshot hooks don't support this version of the game."));
	}
}
