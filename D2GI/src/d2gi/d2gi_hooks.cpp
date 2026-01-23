
#include "../common/common.h"
#include "../common/logger.h"

#include "d2gi.h"
#include "d2gi_device.h"
#include "d2gi_hooks.h"
#include "d2gi_config.h"
#include "d2gi_game_classes.h"

#include "Utils/HookEach.hpp"
#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"

#include "../common/DelimStringReader.hpp"
#include "CPatch/CPatch.h"

// Normally a bad practice, but wincodec.h expects some D3D9 types in the global scope, so provide them.
using namespace D3D9;

#include "ScreenGrab/ScreenGrab9.h"
#include <wincodec.h>

#include <wrl/client.h>

#include <d3dcommon.h>

#include <map>
#include <string>
#include <string_view>
#include <utility>


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
	
	ComPtr<D3D9::IDirect3DSurface9> backbuffer = pD2GI->GetScreenshotSource();
	if (!backbuffer)
	{
		return;
	}

	D3D9::D3DSURFACE_DESC desc;
	if (FAILED(backbuffer->GetDesc(&desc)))
	{
		return;
	}

	if (desc.MultiSampleType != D3D9::D3DMULTISAMPLE_NONE)
	{
		if (FAILED(device->StretchRect(backbuffer.Get(), nullptr, pD2GI->GetBackBufferCopySurface(), nullptr, D3D9::D3DTEXF_NONE)))
		{
			return;
		}
		backbuffer = pD2GI->GetBackBufferCopySurface();
	}

	ComPtr<D3D9::IDirect3DSurface9> buffer;
	if ((desc.Usage & D3DUSAGE_RENDERTARGET) != 0)
	{
		if (FAILED(device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3D9::D3DPOOL_SYSTEMMEM, buffer.GetAddressOf(), nullptr)))
		{
			return;
		}
	
		if (FAILED(device->GetRenderTargetData(backbuffer.Get(), buffer.Get())))
		{
			return;
		}
	}
	else
	{
		buffer = backbuffer;
	}

	CreateDirectoryW(D2GIConfig::GetScreenshotsPath(), nullptr);

	const wchar_t* extension;
	const GUID* imageContainerFormat;
	switch (D2GIConfig::GetScreenshotsFormat())
	{
	case IMG_FORMAT::IMG_PNG:
		extension = L"png";
		imageContainerFormat = &GUID_ContainerFormatPng;
		break;
	case IMG_FORMAT::IMG_JPG:
		extension = L"jpg";
		imageContainerFormat = &GUID_ContainerFormatJpeg;
		break;
	case IMG_FORMAT::IMG_BMP:
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


// ======= Texture UV addressing mode overrides =======
namespace TextureUVFixes
{
	static std::string WcharToUTF8(std::wstring_view str)
	{
		std::string result;

		const int count = WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), nullptr, 0, nullptr, nullptr);
		if (count != 0)
		{
			result.resize(count);
			WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), result.data(), count, nullptr, nullptr);
		}

		return result;
	}

	static std::map<std::string, std::pair<D3D9::D3DTEXTUREADDRESS, D3D9::D3DTEXTUREADDRESS>, std::less<>> s_UVOverrides;
	static bool LoadOverridesData()
	{
		bool bHasOverrides = false;

		constexpr size_t SCRATCH_PAD_SIZE = 32767;
		WideDelimStringReader reader(SCRATCH_PAD_SIZE);
		GetPrivateProfileSection(TEXT("TEXTUREUVOVERRIDES"), reader.PutBuffer(), reader.GetSize(), D2GIConfig::GetConfigFilePath().c_str());

		auto StringToTextureAddress = [](const TCHAR* str)
			{
				if (_tcsicmp(str, TEXT("WRAP")) == 0)
				{
					return D3D9::D3DTADDRESS_WRAP;
				}
				if (_tcsicmp(str, TEXT("CLAMP")) == 0)
				{
					return D3D9::D3DTADDRESS_CLAMP;
				}
				if (_tcsicmp(str, TEXT("MIRROR")) == 0)
				{
					return D3D9::D3DTADDRESS_MIRROR;
				}
				return D3D9::D3DTEXTUREADDRESS(0); // Invalid enum value
			};

		size_t strLength = 0;
		while (const TCHAR* str = reader.GetString(&strLength))
		{
			// Each line should have a format of texName, addressU, addressV
			std::unique_ptr<TCHAR[]> line(_tcsdup(str));

			const TCHAR* separators = TEXT(" ,\t\n");
			TCHAR* context = nullptr;

			TCHAR* texName = _tcstok_s(line.get(), separators, &context);
			const TCHAR* addressUStr = _tcstok_s(nullptr, separators, &context);
			const TCHAR* addressVStr = _tcstok_s(nullptr, separators, &context);
			if (texName == nullptr || addressUStr == nullptr || addressVStr == nullptr)
			{
				continue;
			}

			// Convert the texture name to lowercase
			TCHAR* texCh = texName;
			while (*texCh != '\0')
			{
				*texCh = _totlower(*texCh);
				texCh++;
			}

			const D3D9::D3DTEXTUREADDRESS addressU = StringToTextureAddress(addressUStr);
			const D3D9::D3DTEXTUREADDRESS addressV = StringToTextureAddress(addressVStr);
			if (addressU != D3D9::D3DTEXTUREADDRESS(0) && addressV != D3D9::D3DTEXTUREADDRESS(0))
			{
				s_UVOverrides.try_emplace(WcharToUTF8(texName), addressU, addressV);
				bHasOverrides = true;
			}
		}
		return bHasOverrides;
	}

	static std::string_view GetTextureLookupName(std::string_view moduleName, std::string_view texturePath, char* outResult, std::size_t outResultLen)
	{
		// We want to look up by strings in format 'moduleName.textureName', where textureName is a 'filename' part of texturePath

		// Strip the path part
		const auto lastSlashPos = texturePath.find_last_of('\\');
		if (lastSlashPos != texturePath.npos)
		{
			texturePath.remove_prefix(lastSlashPos + 1);
		}

		// Strip the extension
		const auto dotPos = texturePath.find_last_of('.');
		if (dotPos != texturePath.npos)
		{
			texturePath.remove_suffix(texturePath.length() - dotPos);
		}

		if (moduleName.length() + 1 + texturePath.length() > outResultLen) // +1 for the dot, we don't need the null terminator
		{
			return std::string_view();
		}

		size_t totalLength = moduleName.copy(outResult, moduleName.length());
		outResult[totalLength++] = '.';
		totalLength += texturePath.copy(outResult + totalLength, texturePath.length());

		return std::string_view(outResult, totalLength);;
	}

	static void TryOverrideUV_Internal(D2GI* pD2GI, D3DRenderData* renderData, ResMaterial* mat1)
	{
		if (mat1 != nullptr)
		{
			ResTexture* texture = ResMaterialFacade(mat1).m_texture;
			if (texture != nullptr)
			{
				const char* textureName = ResTextureFacade(texture).m_path;
				const char* moduleName = GameModuleFacade(D3DRenderDataFacade(renderData).m_currentGameModule).m_name;
				if (textureName != nullptr && moduleName != nullptr)
				{
					// Don't use std::string, this is quite a hot code path so avoid allocations
					char textureLookupNameStorage[128];
					const auto it = s_UVOverrides.find(GetTextureLookupName(moduleName, textureName, textureLookupNameStorage, std::size(textureLookupNameStorage)));
					if (it != s_UVOverrides.end())
					{
						pD2GI->EnableUVOverride(it->second.first, it->second.second);
					}
				}
			}
		}
	}

	template<std::size_t Index>
	static void* (__thiscall* orgSetupMaterialsWithBlending)(D3DRenderData* _this, ResMaterial* mat1, void* mat2, void* a3);
	template<std::size_t Index>
	static void* __fastcall SetupMaterialsWithBlending_OverrideUV(D3DRenderData* _this, void*, ResMaterial* mat1, void* mat2, void* a3)
	{
		D2GI* pD2GI = D2GIHookInjector::ObtainD2GI();
		TryOverrideUV_Internal(pD2GI, _this, mat1);

		void* result = orgSetupMaterialsWithBlending<Index>(_this, mat1, mat2, a3);
		pD2GI->DisableUVOverride();

		return result;
	}

	HOOK_EACH_INIT(OverrideUV, orgSetupMaterialsWithBlending, SetupMaterialsWithBlending_OverrideUV);
}


// ======= Texture names injection =======
#ifdef _DEBUG

#pragma comment(lib, "dxguid.lib")

namespace TextureNames
{
	static void SetTextureName(ResTexture* texture)
	{
		ResTextureFacade textureF(texture);

		const char* texName = textureF.m_path;
		D2GISurface* d2giSurface = textureF.m_d3dSurface;

		if (texName != nullptr && d2giSurface != nullptr)
		{
			const size_t texNameLen = std::strlen(texName);
			d2giSurface->SetPrivateData(WKPDID_D3DDebugObjectName, const_cast<char*>(texName), texNameLen, 0);
		}
	}

	template<std::size_t Index>
	static void* (__thiscall* orgTextureInit)(ResTexture* _this, void* a1);
	template<std::size_t Index>
	static void* __fastcall TextureInit_SetName(ResTexture* _this, void*, void* a1)
	{
		void* result = orgTextureInit<Index>(_this, a1);
		SetTextureName(_this);

		return result;
	}

	HOOK_EACH_INIT(SetName, orgTextureInit, TextureInit_SetName);
}
#endif

// ======= Undo patch 8.2's single core affinity changes =======
namespace AffinityChanges
{
	static BOOL WINAPI SetProcessAffinityMask_NOP(HANDLE /*hProcess*/, DWORD_PTR /*dwProcessAffinityMask*/)
	{
		// Do nothing
		return TRUE;
	}
	static auto* const pSetProcessAffinityMask_NOP = &SetProcessAffinityMask_NOP;
}

#pragma comment(lib, "d3d9.lib")

// ======= Batched minimap draws =======
namespace BatchedMinimap
{
	// GraphicsData::GetWindowRect, but we don't bother defining the class
	static RECT* (__thiscall* GraphicsData_GetWindowRect)(const void* _this, RECT* rect);

	static void (__thiscall* orgMapDraw)(void* _this, void* graphics, void* a3);
	static void __fastcall MapDraw_SaveViewport(void* _this, void*, void* graphics, void* a3)
	{
		D2GI* pD2GI = D2GIHookInjector::ObtainD2GI();
		assert(pD2GI != nullptr);

		RECT rect;
		pD2GI->OnMapDrawSetViewport(*GraphicsData_GetWindowRect(graphics, &rect));

		orgMapDraw(_this, graphics, a3);
	}

	static void (*orgBeginScene)();
	static void BeginScene_BeginMinimapDraw()
	{
		orgBeginScene();

		D3DPERF_BeginEvent(D3DCOLOR_RGBA(255, 0, 0, 255), L"Minimap");

		D2GI* pD2GI = D2GIHookInjector::ObtainD2GI();
		assert(pD2GI != nullptr);

		pD2GI->OnBeginMinimapDraw();
	}

	static void (*orgEndScene)();
	static void EndScene_EndMinimapDraw()
	{
		D2GI* pD2GI = D2GIHookInjector::ObtainD2GI();
		assert(pD2GI != nullptr);
		pD2GI->OnEndMinimapDraw();

		D3DPERF_EndEvent();

		orgEndScene();
	}

	static void DrawMinimapLine_Float(void* graphics, float x1, float y1, float x2, float y2, DWORD color, BOOL dontUsePalette)
	{
		D2GI* pD2GI = D2GIHookInjector::ObtainD2GI();
		assert(pD2GI != nullptr);
		pD2GI->OnAddMinimapLine(x1, y1, x2, y2, color);

	}

	static void DrawMinimapLine_Int(void* graphics, int x1, int y1, int x2, int y2, DWORD color, BOOL dontUsePalette)
	{
		DrawMinimapLine_Float(graphics, static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2), color, dontUsePalette);
	}

	static DWORD ConvertRGB_32Bit(DWORD r, DWORD g, DWORD b)
	{
		// Pack in a D3DCOLOR-friendly format right away, without compressing to 16-bit
		return D3DCOLOR_RGBA(r, g, b, 0xFF);
	}

	static __declspec(naked) void ftol_Fake()
	{
		_asm
		{
			sub		esp, 4
			fstp	dword ptr [esp]
			pop		eax
			ret
		}
	}

	template<std::size_t Index>
	static BOOL (*orgAreNotCoordsOnMapGraphicsData)(void* graphics, int x, int y);
	template<std::size_t Index>
	static BOOL AreNotCoordsOnMapGraphicsData_Float(void* graphics, float x, float y)
	{
		return orgAreNotCoordsOnMapGraphicsData<Index>(graphics, static_cast<int>(x), static_cast<int>(y));
	}

	HOOK_EACH_INIT(MapBoundsCheck, orgAreNotCoordsOnMapGraphicsData, AreNotCoordsOnMapGraphicsData_Float);
}


void D2GIHookInjector::InjectHooks(const HookOptions& options)
{
	const TCHAR* c_lpszVersionNames[] =
	{
		TEXT("5.5"),
		TEXT("KotR 1.3"),
		TEXT("8.1"),
		TEXT("8.2"),

		TEXT("Unknown")
	};

	s_eCurrentD2Version = DetectD2Version();

	static_assert(std::size(c_lpszVersionNames) == NUM_D2VERSIONS + 1);
	Logger::Log(TEXT("Detected D2 version: %s"), c_lpszVersionNames[s_eCurrentD2Version]);

	if (!options.m_bEnableHooks)
	{
		Logger::Log(TEXT("Hook injection is not enabled."));
		return;
	}

	using namespace Memory::VP;
	using namespace hook::txn;

	bool bHasTextureFacade = false;
	try
	{
		FACADE_SET_MEMBER_OFFSET(ResTextureFacade, m_d3dSurface, *get_pattern<uint8_t>("89 45 ? E8 ? ? ? ? 8B 45 ? 89 45", 2));

		bHasTextureFacade = true;
	}
	TXN_CATCH();

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
	}


	// Texture UV addressing mode overrides
	if (bHasTextureFacade) try
	{
		using namespace TextureUVFixes;

		std::array<void*, 2> setup_materials_with_blending = {
			get_pattern("E8 ? ? ? ? 8B F8 A1"),
			get_pattern("E8 ? ? ? ? A1 ? ? ? ? 6A ? 6A ? 50 8B 08 FF 51 ? 8B C6"),
		};

		FACADE_SET_MEMBER_OFFSET(ResMaterialFacade, m_texture, *get_pattern<uint8_t>("8B 4D ? BB ? ? ? ? 39 59", 2));
		FACADE_SET_MEMBER_OFFSET(D3DRenderDataFacade, m_currentGameModule, *get_pattern<uint32_t>("8B 81 ? ? ? ? 85 C0 74 ? 8B 80 ? ? ? ? 85 C0 74 ? 51", 2));
		FACADE_SET_MEMBER_OFFSET(GameModuleFacade, m_name, *get_pattern<uint8_t>("8B 46 ? 85 C0 74 ? 50 E8 ? ? ? ? 83 C4 ? 8B 46 ? 85 C0 74 ? 8B 48", 2));

		if (LoadOverridesData())
		{
			HookEach_OverrideUV(setup_materials_with_blending, InterceptCall);
		}
	}
	TXN_CATCH();


	// Undo patch 8.2's single core affinity changes
	if (options.m_bEnableAffinityHooks) try
	{
		auto set_process_affinity_mark = get_pattern("50 FF 15 ? ? ? ? B8 01 00 00 00 C3", 3);

		Patch(set_process_affinity_mark, &AffinityChanges::pSetProcessAffinityMask_NOP);
	}
	TXN_CATCH();


	// Batched minimap draws
	try
	{
		using namespace BatchedMinimap;

		auto begin_scene = get_pattern("E8 ? ? ? ? 85 FF 0F 84 ? ? ? ? B8");
		auto end_scene = get_pattern("E8 ? ? ? ? 8D 4C 24 ? 51 E8 ? ? ? ? 8B 54 24 ? A1 ? ? ? ? 83 C4 ? 3B D0 0F 84");

		std::array<void*, 4> draw_line_int = {

			// Borders
			get_pattern("E8 ? ? ? ? 8B 44 24 ? 83 C4 ? 2B C6"),
			get_pattern("E8 ? ? ? ? 83 C4 ? 6A ? 68 ? ? ? ? 6A ? 6A ? E8 ? ? ? ? 83 C4"),
			get_pattern("E8 ? ? ? ? 83 C4 ? 6A ? 68 ? ? ? ? 6A ? 6A ? E8 ? ? ? ? 8B 4C 24"),
			get_pattern("E8 ? ? ? ? 8B 74 24 ? 83 C4 ? 8B 5C 24"),
		};

		auto draw_line_float = get_pattern("55 57 52 E8 ? ? ? ? 83 C4 1C 5F", 3);

		std::array<void*, 5> convert_rgb_32bit = {

			// Borders
			get_pattern("E8 ? ? ? ? 8B 4D ? 83 C4 ? 50"),
			get_pattern("E8 ? ? ? ? 8B 54 24 ? 83 C4 ? 50 8B 45"),
			get_pattern("E8 ? ? ? ? 83 C4 ? 50 8B 44 24 ? 50 57"),
			get_pattern("E8 ? ? ? ? 8B 4C 24 ? 83 C4 ? 50 56"),

			// Map segments
			get_pattern("E8 ? ? ? ? 8B 54 24 ? 83 C4 ? 50 56"),
		};

		auto map_draw = get_pattern("E8 ? ? ? ? 8B 15 ? ? ? ? 39 BA ? ? ? ? 0F 84");

		auto map_segment_draw_ftol = pattern("E8 ? ? ? ? DD 44 24 ? DC 4E ? 8B F8 DC 44 24 ? E8 ? ? ? ? DD 44 24 ? DC 4E ? 8B D8 DC 44 24 ? E8 ? ? ? ? DD 44 24 ? DC 4E ? 8B E8 DC 44 24 ? E8").get_one();
		std::array<void*, 4> map_segment_draw_ftol_calls = {
			map_segment_draw_ftol.get<void>(0),
			map_segment_draw_ftol.get<void>(0x12),
			map_segment_draw_ftol.get<void>(0x24),
			map_segment_draw_ftol.get<void>(0x36),
		};

		auto coords_map_bounds_check = pattern("E8 ? ? ? ? 83 C4 ? 85 C0 75 ? 8B 4C 24 ? 56 53 51 E8").get_one();
		std::array<void*, 2> coords_map_bounds_check_calls = {
			coords_map_bounds_check.get<void>(0),
			coords_map_bounds_check.get<void>(0x13),
		};

		auto get_window_rect = get_pattern("8B 44 24 ? 33 D2 66 8B 51 ? 33 F6", -8);
		GraphicsData_GetWindowRect = reinterpret_cast<decltype(GraphicsData_GetWindowRect)>(get_window_rect);

		InterceptCall(begin_scene, orgBeginScene, BeginScene_BeginMinimapDraw);
		InterceptCall(end_scene, orgEndScene, EndScene_EndMinimapDraw);

		for (void* addr : draw_line_int)
		{
			InjectHook(addr, DrawMinimapLine_Int);
		}
		InjectHook(draw_line_float, DrawMinimapLine_Float);

		for (void* addr : convert_rgb_32bit)
		{
			InjectHook(addr, ConvertRGB_32Bit);
		}

		InterceptCall(map_draw, orgMapDraw, MapDraw_SaveViewport);

		// Change GraphicsData::DrawLine arguments inside MapSegment::Draw from int to float to preserve precision
		for (void* addr : map_segment_draw_ftol_calls)
		{
			InjectHook(addr, ftol_Fake);
		}

		HookEach_MapBoundsCheck(coords_map_bounds_check_calls, InterceptCall);
	}
	TXN_CATCH();


	Logger::Log(TEXT("Injected common hooks."));

	if (s_eCurrentD2Version != D2V_5_5 && s_eCurrentD2Version != D2V_UNKNOWN) {
		//hooks ONLY for v1.3, v8.1 and v8.2

		//Screenshot save patch
		D2GIHookInjector::InjectScreenshotsPatch();

		//Interface aspect fix
		if (options.m_bEnableUIHooks)
			D2GIHookInjector::InjectInterfacePatch();

	} else {
		Logger::Log(TEXT("Interface and screenshot hooks don't support this version of the game."));
	}

	// Texture names injection
#ifdef _DEBUG
	if (bHasTextureFacade) try
	{
		using namespace TextureNames;

		std::array<void*, 3> texture_init = {
			get_pattern("E8 ? ? ? ? 8B 45 ? 46 3B F0 7C ? 8B 0D"),
			get_pattern("E8 ? ? ? ? 85 C0 75 ? 8B 4C 24 ? 51"),
			get_pattern("E8 ? ? ? ? 8B 45 ? 46 3B F0 7C ? 68"),
		};
		HookEach_SetName(texture_init, InterceptCall);
	}
	TXN_CATCH();
#endif
}
