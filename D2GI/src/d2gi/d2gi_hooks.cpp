
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

int  m_dwGameVersion;
int* m_pViewerPtr;


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


int D2GIHookInjector::GetGameVersion() try {
	using namespace hook::txn;

	// v x.x (v 8.0, v 5.8, etc.)
	char* gameVersionStr = (char*)get_pattern_uintptr("76 20 ? 2E ?");

	float temp_flt;
	int scan = sscanf(gameVersionStr, "%*s %f", &temp_flt);

	if (scan != 1)
		return 0;

	//string "8.1" -> float 8.1 -> int 81
	int versionIdx = temp_flt * 10;

	//This code does rough check of the year in the "C:\Nek\VRAPPLVS\TRUCK\aeffects.cpp  Dec  4 2007" string:
	//in 8.0 - 8.1 there is 2003, and in 8.2 there is 2007.
	if (CPatch::GetInt(0x6724FF) == 925904946) {
		versionIdx = 82;
	}

	//EU KotR releases (1.0 - 1.3)
	//This is done because the patterns for 8.0 - 8.2 and 1.3 are the same.
	if (versionIdx <= 14 && versionIdx >= 10) {
		versionIdx = 80;
	}

	return versionIdx;
}
catch (const hook::txn_exception&)
{
	return 0;
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

void D2GIHookInjector::InjectScreenshotsPatch() try {
	using namespace hook::txn;

	int addr_FuncStart;
	int addr_WriteAddr;
	int addr_mkdir;
	int addr_fopen;
	int addr_fclose;

	//6.6 - 6.9
	if (m_dwGameVersion < 70) {
		addr_FuncStart = (int)pattern("68 ? ? ? ? E8 ? ? ? ? 8B 15 ? ? ? ? 8D 44 24 ? 52 68 ? ? ? ? 68").get_first();
		addr_WriteAddr = addr_FuncStart + 55;
		addr_mkdir = addr_FuncStart + 5;
		addr_fopen = addr_FuncStart + 47;
		addr_fclose = addr_FuncStart + 61;

	//7.0 only
	} else if (m_dwGameVersion == 70) {
		//596706
		addr_FuncStart = (int)pattern("68 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 8D 4C 24 4C 50 68 ? ? ? ? 68").get_first();
		//59673C
		addr_WriteAddr = addr_FuncStart + 54;
		//59670B
		addr_mkdir = addr_FuncStart + 5;
		//596734
		addr_fopen = addr_FuncStart + 46;
		//596742
		addr_fclose = addr_FuncStart + 60;

	//7.1 - 8.2
	} else {
		//576418 in 8.2
		addr_FuncStart = (int)pattern("56 68 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ?").get_first();
		
		//7.1 - 7.3
		if (m_dwGameVersion < 74) {
			//offsets for 7.3
			//funcStart - 576528
			//57655F
			addr_WriteAddr = addr_FuncStart + 55;
			//57652E
			addr_mkdir = addr_FuncStart + 6;
			//576557
			addr_fopen = addr_FuncStart + 47;
			//576565
			addr_fclose = addr_FuncStart + 61;
		
		//7.4 - 8.2
		} else {
			//offsets for 8.2
			//576452
			addr_WriteAddr = addr_FuncStart + 58;
			//57641E
			addr_mkdir = addr_FuncStart + 6;
			//57644A
			addr_fopen = addr_FuncStart + 50;
			//576458
			addr_fclose = addr_FuncStart + 64;
		}
	}

	CPatch::RedirectCall(addr_WriteAddr, &WriteScreenshotFunc);

	//_mkdir(".\\screenshots");
	CPatch::Nop(addr_mkdir, 5);

	//v3 = fopen(&Dest, "wb");
	CPatch::Nop(addr_fopen, 5);

	//fclose(v3);
	CPatch::Nop(addr_fclose, 5);

	Logger::Log(TEXT("Injected screenshots hook."));
} catch (const hook::txn_exception&) {
	Logger::Log(TEXT("Failed to inject screenshots hook."));
}

/////* mirrors hooks */////
// Mirrors hooks code
int m_funcAddr_ProcessMirrors;
int m_callAddr_ProcessMirrors;

int m_funcAddr_CameraInstanceReset;
int m_callAddr_CameraInstanceReset;

bool* m_bIsMirrorsProcessed;
float* m_fCabinCameraRotation;

void __fastcall ProcessMirrors(int* _this, int* EDX) {
	bool temp = *m_bIsMirrorsProcessed;

	float camRotMin = 0.2;
	float cameraRot = *m_fCabinCameraRotation;
	
	// If the interior camera has a small rotation angle,
	// then disable the rendering of the side mirrors
	if (abs(cameraRot) < camRotMin)
		*m_bIsMirrorsProcessed = false;

	//call original function
	((int(__thiscall*)(int*))m_funcAddr_ProcessMirrors)(_this);

	*m_bIsMirrorsProcessed = temp;
}


//Called on Viewer::attach_vehicle() -> CameraInstance::Reset(), just after
//adjusting cabine and side mirrors matrices

int* __fastcall OnCameraInstanceReset(int* _this, int edx) {
	float* cabineMtx = (float*)((char*)*m_pViewerPtr + 0x208);
	float* mirrorsMtx = (float*)((char*)*m_pViewerPtr + 0x238);

	float addVal_y = 0.2;
	float addVal_z = -0.15;

	//cabineMtx->pos.z
	if (cabineMtx[11] < 1.0) {
		addVal_y = 0.27;
		addVal_z = -0.28;
	}

	//mirrorsMtx->pos.y, pos.z
	mirrorsMtx[10] += addVal_y;
	mirrorsMtx[11] += addVal_z;

	//call original function
	return ((int* (__thiscall*)(int*))m_funcAddr_CameraInstanceReset)(_this);
}

//Used to convert uint32 value into pattern for ModUtils
char* UInt32toPattern(uint32_t value) {
	char result[24];
	sprintf(result, "%02X %02X %02X %02X", (value & 0xFF), ((value >> 8) & 0xFF), ((value >> 16) & 0xFF), ((value >> 24) & 0xFF));
	return result;
}

void D2GIHookInjector::InjectMirrorsPatch() try {
	using namespace Memory::VP;
	using namespace hook::txn;

	m_callAddr_CameraInstanceReset = get_pattern_uintptr("E8 ? ? ? ? 89 1D ? ? ? ? 8B 8D ? ? ? ? 51 8B CD ");
	m_funcAddr_CameraInstanceReset = (int)ReadCallFrom(m_callAddr_CameraInstanceReset);

	m_funcAddr_ProcessMirrors = get_pattern_uintptr("55 8B EC 83 E4 F8 81 EC ? ? ? ? A1 ? ? ? ? 53 55 56 33 F6 57 3B C6 8B");
	// It won't be possible to create a pattern before finding function - the only call to this function occurs when
	// rendering viewport elements, i.e. there is a cycle with a call to obj[i]->Draw(...)
	m_callAddr_ProcessMirrors = get_pattern_uintptr(UInt32toPattern((int)m_funcAddr_ProcessMirrors));

	m_bIsMirrorsProcessed = *(bool**)pattern("DC 1D ? ? ? ? DF E0 F6 C4 01 0F 84 ? ? ? ? A1 ? ? ? ?").get_first(18);
	m_fCabinCameraRotation = *(float**)pattern("D8 05 ? ? ? ? D9 44 24 00").get_first(2);

	CPatch::RedirectCall(m_callAddr_CameraInstanceReset, &OnCameraInstanceReset);
	CPatch::SetPointer(m_callAddr_ProcessMirrors, &ProcessMirrors);

	Logger::Log(TEXT("Successfully injected cab mirrors hooks"));
}
catch (const hook::txn_exception&)
{
	Logger::Log(TEXT("Failed to inject cab mirrors hooks."));
}


/////* Interface hooks variables */////

//Selected item ID in mainMenu->graphics->screenResolution
int m_dwMenuSettingsValue;
//Interface aspect (for interiors FOV fix)
float m_fUIAspect;
//Game window resolution forced by D2GI
int m_dwWinResX, m_dwWinResY;
//UI forced resolution
int m_dwUIResX, m_dwUIResY;

static MenuGraphics** pMenuGraphics;

static void (*orgOnPrepareStartGame)(void* a1, void* a2);
void D2GIHookInjector::OnPrepareStartGame(void* a1, void* a2) {
	//MenuVideo->overwrite selected resolution item ID
	MenuGraphicsFacade(*pMenuGraphics).m_videoIni = m_dwMenuSettingsValue;

	//run original function
	orgOnPrepareStartGame(a1, a2);
}

static InterfaceHookOffsets InterfaceOffsets;

static int (__thiscall *orgOnInitDrawForGame)(int* CWinApp, int width, int height, int depth, int a5);
signed int __fastcall D2GIHookInjector::OnInitDrawForGame(int* CWinApp, int EDX, int width, int height, int depth, int a5) {
	D2GI* pD2GI = D2GIHookInjector::ObtainD2GI();

	if (pD2GI == NULL) {
		// In this case, the output to the log is disabled, because InitDrawForGame is called both
		// when the game window starts and when the game world is loaded, even when pD2GI = NULL
		
		//Logger::Log(TEXT("OnInitDrawForGame->pD2GI==NULL"));

		return orgOnInitDrawForGame(CWinApp, width, height, depth, a5);
	}

	std::vector<D3D9::D3DDISPLAYMODE> modes = pD2GI->GetDisplayModes();

	//Check resolutions from largest to smallest
	for (auto mode = modes.rbegin(); mode != modes.rend(); mode++)
	{
		int modeX = mode->Width;
		int modeY = mode->Height;

		//Check if the resolution[i] is greater than the size of the game window
		if (modeX > m_dwWinResX || modeY > m_dwWinResY)
			continue;

		//Check if the resolution[i] exceeds the maximum allowed interface size
		if (modeX > 1600 || modeY > 1200)
			continue;


		m_dwUIResX = modeX;
		m_dwUIResY = modeY;


		//Replace default 1024x768 resolution to new
		CPatch::SetShort(InterfaceOffsets.addr_resX, m_dwUIResX);
		CPatch::SetShort(InterfaceOffsets.addr_resY, m_dwUIResY);

		m_fUIAspect = (float)m_dwUIResX / (m_dwUIResY);
		Logger::Log(TEXT("Set interface resolution to %dx%d"), m_dwUIResX, m_dwUIResY);

		//Set panel offsets (align the panel to the right edge of the game window)
		CPatch::SetInt(InterfaceOffsets.addr_pagerX, m_dwUIResX -  439);
		CPatch::SetInt(InterfaceOffsets.addr_pagerY,                11);
		CPatch::SetInt(InterfaceOffsets.addr_panelX, m_dwUIResX - 1600);
		CPatch::SetInt(InterfaceOffsets.addr_textX,  m_dwUIResX - 1131);

		return orgOnInitDrawForGame(CWinApp, width, height, depth, a5);
	}
}

static int* MenuBackInfoX;
static int* MainSideBarX;

static void (*orgInitInGameMenus)();
static void InitInGameMenus_FixupOffsets()
{
	//fix sidebar positions
	*MenuBackInfoX = m_dwUIResX - 385;
	*MainSideBarX = m_dwUIResX - 225;

	orgInitInGameMenus();
}

static CBlockObserver** pCabObserver;

static void (*orgOnInitClusters)();
void D2GIHookInjector::OnInitClusters() {

	CBlockObserverFacade CabObserverF(*pCabObserver);

	//blockObserver->FOV
	CabObserverF.m_fovX = m_fUIAspect * 1.2f;
	CabObserverF.m_fovY = 1.2f;

	//call original function
	orgOnInitClusters();
}

//Interface main injection code
void D2GIHookInjector::InjectInterfacePatch(bool use_mirrors_fix) try {
	// How it works:
	// 1. The screen resolution set by D2GI is checked; if its width is higher than 1600, the width
	// becomes equal to 1600, the height is proportionally reduced. If the resolution is less than
	// 1280x720, then the then the hook injection will be aborted.
	// 
	// 
	// 2. Next comes the check: if the screen resolution is 4:3, then the then the hook injection will
	// be aborted too. If the format resolution is 16:9 or higher, then the interface resolution will
	// be in the range from 1280x720 to 1600x1200.
	//
	//
	// 3. Installation of hooks:
	// * OnPrepareStartGame - here there is a forced selection of the "1024x768" button in the game
	// graphics settings menu;
	// 
	// * InitInGameMenus    - here new offsets of the interface on the right side of the screen are set;
	// 
	// * OnInitClusters     - a new interior FOV is being installed here, because the standard D2GI fix
	// not actual due to interface hooks;
	//
	// * OnInitDrawForGame - here set the interface resolution depending on what is selected in the D2GI
	// settings and what the display supports. The default 1024x768 will be overwritten by the new values.
	//
	//
	// All this hook is made as a redesign of the 1024x768 resolution interface layout.


	//1) Check resolution
	m_dwWinResX = D2GIConfig::GetVideoWidth();
	m_dwWinResY = D2GIConfig::GetVideoHeight();

	if (m_dwWinResX < 1280 || m_dwWinResY < 720) {
		Logger::Log(TEXT("Interface hooks injection aborted: screen resolution is too low (at least 1280x720 required)"));
		return;
	}

	// First match patterns before we try to do any patching
	using namespace hook::txn;

	auto init_in_game_menus = get_pattern("E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 0D ? ? ? ? 8B 81");

	auto sidebar_positions = pattern("C7 05 ? ? ? ? ? ? ? ? 8B C8 C7 05 ? ? ? ? ? ? ? ? 8B D0 E9").get_one();
	MainSideBarX = *sidebar_positions.get<int*>(2);
	MenuBackInfoX = *sidebar_positions.get<int*>(0xC + 2);

	m_dwUIResX = m_dwWinResX;
	m_dwUIResY = m_dwWinResY;

	float real_aspect = (float)m_dwUIResX / (float)m_dwUIResY;
	float aspect_rev = (float)m_dwUIResY / (float)m_dwUIResX;


	//1) Clamp resolution to 1600; max in-game GUI textures size is 1600x1200, in other cases
	//textures in esc16.res should be redrawn for higher resolutions.
	if (m_dwUIResX > 1600) {
		m_dwUIResX = 1600;
		m_dwUIResY = (int)((float)m_dwUIResX / real_aspect);
	}

	//Get offsets using patterns
	//Top UI panel
	int addr_panelDrawStart = get_pattern_uintptr("C7 44 24 ? ? ? ? ? 89 4C 24 ? ? ? ? ? ? ? ? ? ? ? 7E 19 C7 44 24 ? ? ? ? ? C7 44 24");

	InterfaceOffsets.addr_cmp1204 = addr_panelDrawStart - 4;
	InterfaceOffsets.addr_cmp800  = addr_panelDrawStart + 50;
	InterfaceOffsets.addr_pagerX  = addr_panelDrawStart + 90;
	InterfaceOffsets.addr_pagerY  = addr_panelDrawStart - 9;
	InterfaceOffsets.addr_panelX  = addr_panelDrawStart + 104;
	InterfaceOffsets.addr_textX   = addr_panelDrawStart + 99;

	InterfaceOffsets.addr_panelDrawStart = addr_panelDrawStart;

	//Settings menu offsets
	InterfaceOffsets.addr_resY = (int)pattern("E8 ? ? ? ? 0B ? 80 CC 40 50 ? 68 00 03 00 00 68 00 04 00 00").get_first(13);
	InterfaceOffsets.addr_resX = InterfaceOffsets.addr_resY + 5;

	//Functions call offsets
	int callAddr_prepareGame, callAddr_initClusters, callAddr_initDraw;

	callAddr_prepareGame  = get_pattern_uintptr("E8 ? ? ? ? A1 ? ? ? ? 83 C4 08 3B C3 75 0E 53");
	callAddr_initClusters = get_pattern_uintptr("E8 ? ? ? ? DD 86 ? ? ? ? 8B CE D9 9E ? ? ? ? DD 86");
	callAddr_initDraw     = (int)pattern("8B 7C 24 1C 55 52 53 57 8B CE E8 ? ? ? ? 85 C0").get_first(10); //0x5DD8D8 in 8.2

	//2) Set resolution from aspect
	if (aspect_rev > 0.7) {
		Logger::Log(TEXT("Interface hooks injection aborted: the game is not running in widescreen format"));
		return;
	} else {
		//Called from InjectInterfacePatch because it works correct only if UI patched
		if (use_mirrors_fix && m_pViewerPtr) {
			D2GIHookInjector::InjectMirrorsPatch();
		}

		//This is done to force the game to take panel offsets from only one address
		CPatch::SetInt(InterfaceOffsets.addr_cmp1204, INT_MAX);
		CPatch::SetInt(InterfaceOffsets.addr_cmp800,  INT_MAX);
	}


	//Select 1024x768 in the game settings.
	//Possible values:
	// hex   int 
	//0x110 (274) 640x480
	//0x120 (290) 800x600
	//0x130 (306) 1024x768
	//0x140 (322) 1600x1200

	m_dwMenuSettingsValue = 306;


	using namespace Memory::VP;

	//3) hook functions
	InterceptCall(callAddr_prepareGame, orgOnPrepareStartGame, OnPrepareStartGame);
	InterceptCall(init_in_game_menus, orgInitInGameMenus, InitInGameMenus_FixupOffsets);
	InterceptCall(callAddr_initClusters, orgOnInitClusters, &OnInitClusters);
	InterceptCall(callAddr_initDraw, orgOnInitDrawForGame, &OnInitDrawForGame);

	Logger::Log(TEXT("Successfully injected interface hooks"));
} catch (const hook::txn_exception&) {
	Logger::Log(TEXT("Failed to inject interface hooks."));
}


// ======= Fix horizontal raindrops at over 50 FPS =======
namespace RaindropsFix
{
	int ftol_PreserveFrac(double val)
	{
		static double lastFraction = 0.0;

		double integral;
		lastFraction = std::modf(val + lastFraction, &integral);

		return static_cast<int>(integral);
	}

	__declspec(naked) void ftol_PreserveFracHook()
	{
		_asm
		{
			sub		esp, 8
			fstp	qword ptr [esp]
			call	ftol_PreserveFrac
			add		esp, 8
			retn
		}
	}
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

		// With this scene scope, the blitter won't start/end scene for every single minimap icon
		auto SceneScope = pD2GI->BeginSceneScope();

		RECT rect;
		pD2GI->OnMapDrawSetViewport(*GraphicsData_GetWindowRect(graphics, &rect));

		orgMapDraw(_this, graphics, a3);

		// Reset the minimap offsets
		D2GI::m_LastBltX = D2GI::m_LastBltY = 0.0f;
	}

	static void (*orgBeginScene)();
	static void BeginScene_BeginMinimapDraw()
	{
		orgBeginScene();

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

	// Subpixel minimap scrolling
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


	// Subpixel minimap icon scrolling
	template<std::size_t Index>
	static void* orgftol_StoreX;

	template<std::size_t Index>
	static __declspec(naked) void ftol_StoreX()
	{
		static void** const func = &orgftol_StoreX<Index>;
		_asm
		{
			fst		[D2GI::m_LastBltX]
			mov		eax, [func]
			jmp		dword ptr [eax]
		}
	}

	template<std::size_t Index>
	static void* orgftol_StoreY;

	template<std::size_t Index>
	static __declspec(naked) void ftol_StoreY()
	{
		static void** const func = &orgftol_StoreY<Index>;
		_asm
		{
			fst		[D2GI::m_LastBltY]
			mov		eax, [func]
			jmp		dword ptr [eax]
		}
	}

	HOOK_EACH_INIT(StoreFloatX, orgftol_StoreX, ftol_StoreX);
	HOOK_EACH_INIT(StoreFloatY, orgftol_StoreY, ftol_StoreY);
}

// ======= Fixed North Harbor bridge lighting =======
namespace BridgeLightingFix
{
	static void* (*orgOperatorNew)(size_t size);
	static void* operatorNew_InitializePSize(size_t size)
	{
		void* result = orgOperatorNew(size);

		if (result != nullptr)
		{
			const float scaledPointSize = D2GIHookInjector::ObtainD2GI()->GetForcedHeight() / 480.0f; // Keep constant size relative to the lowest base resolution

			D3D7::D3DLVERTEX* verts = static_cast<D3D7::D3DLVERTEX*>(result);
			const size_t numVerts = size / sizeof(*verts);
			for (size_t i = 0; i < numVerts; i++)
			{
				*reinterpret_cast<float*>(&verts[i].dwReserved) = scaledPointSize;
			}
		}

		return result;
	}

	// Function definition perfectly matching DrawPrimitive
	static COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawBridgeLightingHook(D2GIDevice* device, D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID pVerts, DWORD dwVertCount, DWORD dwFlags)
	{
		device->GetD2GI()->OnDrawBridgeLightingPrimitive(pt, dwFVF, pVerts, dwVertCount, dwFlags);
		return DD_OK;
	}
}

void D2GIHookInjector::InjectHooks(const HookOptions& options)
{
	m_dwGameVersion = GetGameVersion();

	if (!m_dwGameVersion) {
		Logger::Log(TEXT("Failed to detect D2 version"));
	} else {
		Logger::Log(TEXT("Detected D2 version: %d.%d"), (m_dwGameVersion / 10) % 10, m_dwGameVersion % 10);
	}

	if (!options.m_bEnableHooks) {
		Logger::Log(TEXT("Hook injection is not enabled."));
		return;
	}

	Logger::Log(TEXT("Initializing facades and applying hooks..."));

	using namespace Memory::VP;
	using namespace hook::txn;

	bool bHasTextureFacade = false;
	try {
		FACADE_SET_MEMBER_OFFSET(ResTextureFacade, m_d3dSurface, *get_pattern<uint8_t>("89 45 ? E8 ? ? ? ? 8B 45 ? 89 45", 2));

		bHasTextureFacade = true;

		Logger::Log(TEXT("Texture facade initialized"));
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to read texture facade offsets"));
	}


	bool bHasMenuGraphics = false;
	try {
		pMenuGraphics = *get_pattern<MenuGraphics**>("A1 ? ? ? ? 0F BF 88 ? ? ? ? 0F BF 90 ? ? ? ? 89 4C 24", 1);

		FACADE_SET_MEMBER_OFFSET(MenuGraphicsFacade, m_videoIni, *get_pattern<uint32_t>("89 87 ? ? ? ? 75", 2));

		bHasMenuGraphics = true;

		Logger::Log(TEXT("Menu graphics facade initialized"));
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to read menu graphics facade offsets"));
	}


	bool bHasBlockObserver = false;
	try {
		// pattern for versions 6.6 - 8.2
		auto fov_xy = pattern("89 48 58 89 48 54 89 ? 60 89 ? 5C").get_one();

		pCabObserver = *get_pattern<CBlockObserver**>("8B 15 ? ? ? ? 8D 4C 24 ? 51 6A", 2);

		FACADE_SET_MEMBER_OFFSET(CBlockObserverFacade, m_fovX, *fov_xy.get<uint8_t>(2));
		FACADE_SET_MEMBER_OFFSET(CBlockObserverFacade, m_fovY, *fov_xy.get<uint8_t>(3 + 2));

		bHasBlockObserver = true;

		Logger::Log(TEXT("Block observer facade initialized"));
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to read block observer facade offsets"));
	}

	try {
		auto device_address_ptr = get_pattern<D3D7::IDirect3DDevice7**>("8B 0D ? ? ? ? 8D 54 24 ? 51 6A ? 52 68 ? ? ? ? 68 ? ? ? ? C7 44 24", 2);
		auto setup_transforms = get_pattern("50 51 8B CE E8 ? ? ? ? 5F 5E 5D", 4);

		m_deviceAddress = *device_address_ptr;
		InterceptCall(setup_transforms, m_origSetupTransform, &SetupTransforms);

		Logger::Log(TEXT("Aspect ratio fix injected"));
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to inject aspect ratio fix"));
	}


	// Texture UV addressing mode overrides
	if (bHasTextureFacade) try {
		using namespace TextureUVFixes;

		std::array<void*, 2> setup_materials_with_blending = {
			get_pattern("E8 ? ? ? ? 8B F8 A1"),
			get_pattern("E8 ? ? ? ? A1 ? ? ? ? 6A ? 6A ? 50 8B 08 FF 51 ? 8B C6"),
		};

		FACADE_SET_MEMBER_OFFSET(ResMaterialFacade, m_texture, *get_pattern<uint8_t>("8B 4D ? BB ? ? ? ? 39 59", 2));
		FACADE_SET_MEMBER_OFFSET(D3DRenderDataFacade, m_currentGameModule, *get_pattern<uint32_t>("8B 81 ? ? ? ? 85 C0 74 ? 8B 80 ? ? ? ? 85 C0 74 ? 51", 2));
		FACADE_SET_MEMBER_OFFSET(GameModuleFacade, m_name, *get_pattern<uint8_t>("8B 46 ? 85 C0 74 ? 50 E8 ? ? ? ? 83 C4 ? 8B 46 ? 85 C0 74 ? 8B 48", 2));

		if (LoadOverridesData()) {
			HookEach_OverrideUV(setup_materials_with_blending, InterceptCall);
			Logger::Log(TEXT("Texture addressing overrides injected"));
		} else {
			Logger::Log(TEXT("Configuration for texture addressing overrides isn't set, skipping overrides injection"));
		}
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to inject texture addressing overrides"));
	}


	// Undo patch 8.2's single core affinity changes
	if (options.m_bEnableAffinityHooks) try {
		auto set_process_affinity_mark = get_pattern("50 FF 15 ? ? ? ? B8 01 00 00 00 C3", 3);

		Patch(set_process_affinity_mark, &AffinityChanges::pSetProcessAffinityMask_NOP);

		Logger::Log(TEXT("Process affinity fix injected"));
	} catch (const hook::txn_exception&) {
		if (m_dwGameVersion == 82) // log only if somehow failed in 8.2, in other versions should fail always
			Logger::Log(TEXT("Failed to inject process affinity fix"));
	}


	// Batched minimap draws
	try {
		using namespace BatchedMinimap;

		auto begin_scene = get_pattern("E8 ? ? ? ? 85 FF 0F 84 ? ? ? ? B8");
		auto end_scene = [] {
			try
			{
				// 8.2/KotR 1.3
				 return get_pattern("E8 ? ? ? ? 8D 4C 24 ? 51 E8 ? ? ? ? 8B 54 24 ? A1 ? ? ? ? 83 C4 ? 3B D0 0F 84");
			}
			catch (const hook::txn_exception&)
			{
				// 7.3 LT
				return get_pattern("E8 ? ? ? ? 8D 4C 24 ? 51 E8 ? ? ? ? 8B 00 8B 0D ? ? ? ? 83 C4 ? 3B C1 89 44 24 ? 0F 84");
			}
		}();

		auto draw_border_line1 = pattern("E8 ? ? ? ? 83 C4 ? 6A ? 68 ? ? ? ? 6A ? 6A ? E8").count(2);
		std::array<void*, 4> draw_line_int = {

			// Borders
			get_pattern("E8 ? ? ? ? 8B 44 24 ? 83 C4 ? 2B C6"),
			draw_border_line1.get(0).get<void>(0),
			draw_border_line1.get(1).get<void>(0),
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

		auto get_window_rect = [] {
			try
			{
				// 8.2/KotR 1.3
				return get_pattern("8B 44 24 ? 33 D2 66 8B 51 ? 33 F6", -8);
			}
			catch (const hook::txn_exception&)
			{
				// 7.3 LT
				return get_pattern("8B 44 24 ? 53 57 33 FF", -8);
			}
		}();

		// Smooth minimap icon scrolling
		// Separate try...catch, as it is completely optional and other minimap changes will work without it.
		try {
			// Most of those patterns differ between 8.x and earlier executables, so branch the code instead of just the individual pattern matches.
			auto other_vehicles = pattern("E8 ? ? ? ? DC 4C 24 ? 8B F0 DC 44 24 ? DC 25 ? ? ? ? E8").get_one();
			auto circuit = pattern("E8 ? ? ? ? DD 44 24 ? DC 0D ? ? ? ? 8B F0 DC 44 24 ? E8").get_one();
			auto player_marker = pattern("E8 ? ? ? ? DD 44 24 ? DC ? ? ? ? ? 50 E8").count(2);
			try
			{
				// 8.2/KotR 1.3
				auto town_icons = pattern("E8 ? ? ? ? D9 44 24 ? DC 4C 24 ? 8B F8 DC 44 24 ? E8").count(2);
				auto obstacles = pattern("E8 ? ? ? ? DC 4C 24 ? 8B F0 DC 44 24 ? E8").count(2);
				auto repair_shops = pattern("E8 ? ? ? ? D9 44 24 ? DC 4C 24 ? 8B F0 DC 44 24 ? E8").count(3);

				std::array<void*, 11> store_icon_posx = {
					other_vehicles.get<void>(0), // Other vehicles
					town_icons.get(0).get<void>(0), // Town/gas station icons
					town_icons.get(1).get<void>(0), // Selected hired driver
					obstacles.get(0).get<void>(0), // Obstacles/road works
					obstacles.get(1).get<void>(0), // Town names
					circuit.get<void>(0), // Circuit icon and text
					repair_shops.get(0).get<void>(0), // Repair shops and parkings
					repair_shops.get(1).get<void>(0), // Other players and hired drivers
					repair_shops.get(2).get<void>(0), // Cargo destination masks
					player_marker.get(0).get<void>(0x10), // Player icon
					player_marker.get(1).get<void>(0x10), // Player name
				};

				std::array<void*, 11> store_icon_posy = {
					other_vehicles.get<void>(0x15), // Other vehicles
					town_icons.get(0).get<void>(0x13), // Town/gas station icons
					town_icons.get(1).get<void>(0x13), // Selected hired driver
					obstacles.get(0).get<void>(0xF), // Obstacles/road works
					obstacles.get(1).get<void>(0xF), // Town names
					circuit.get<void>(0x15), // Circuit icon and text
					repair_shops.get(0).get<void>(0x13), // Repair shops and parkings
					repair_shops.get(1).get<void>(0x13), // Other players and hired drivers
					repair_shops.get(2).get<void>(0x13), // Cargo destination masks
					player_marker.get(0).get<void>(0), // Player icon
					player_marker.get(1).get<void>(0), // Player name
				};

				HookEach_StoreFloatX(store_icon_posx, InterceptCall);
				HookEach_StoreFloatY(store_icon_posy, InterceptCall);
			}
			catch (const hook::txn_exception&)
			{
				// 7.3 LT
				auto town_icons = pattern("E8 ? ? ? ? DC 4C 24 ? 8B F0 89 74 24 ? DC 44 24 ? E8").get_one();
				auto repair_shops = pattern("E8 ? ? ? ? DC 4C 24 ? 8B F8 DC 44 24 ? E8").count(3);
				auto obstacles = pattern("E8 ? ? ? ? DC 4C 24 ? 8B F0 DC 44 24 ? E8").count(3);

				// 7.3 LT
				std::array<void*, 11> store_icon_posx = {
					other_vehicles.get<void>(0), // Other vehicles
					town_icons.get<void>(0), // Town/gas station icons
					repair_shops.get(2).get<void>(0), // Selected hired driver
					obstacles.get(0).get<void>(0), // Obstacles/road works
					repair_shops.get(1).get<void>(0), // Town names
					circuit.get<void>(0), // Circuit icon and text
					repair_shops.get(0).get<void>(0), // Repair shops and parkings
					obstacles.get(1).get<void>(0), // Other players and hired drivers
					obstacles.get(2).get<void>(0), // Cargo destination masks
					player_marker.get(0).get<void>(0x10), // Player icon
					player_marker.get(1).get<void>(0x10), // Player name
				};

				std::array<void*, 11> store_icon_posy = {
					other_vehicles.get<void>(0x15), // Other vehicles
					town_icons.get<void>(0x13), // Town/gas station icons
					repair_shops.get(2).get<void>(0xF), // Selected hired driver
					obstacles.get(0).get<void>(0xF), // Obstacles/road works
					repair_shops.get(1).get<void>(0xF), // Town names
					circuit.get<void>(0x15), // Circuit icon and text
					repair_shops.get(0).get<void>(0xF), // Repair shops and parkings
					obstacles.get(1).get<void>(0xF), // Other players and hired drivers
					obstacles.get(2).get<void>(0xF), // Cargo destination masks
					player_marker.get(0).get<void>(0), // Player icon
					player_marker.get(1).get<void>(0), // Player name
				};

				HookEach_StoreFloatX(store_icon_posx, InterceptCall);
				HookEach_StoreFloatY(store_icon_posy, InterceptCall);
			}

			Logger::Log(TEXT("Smooth minimap icons scrolling injected"));
		} catch (const hook::txn_exception&) {
			Logger::Log(TEXT("Failed to inject smooth minimap icons scrolling"));
		}

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

		Logger::Log(TEXT("Batched minimap drawing injected"));
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to inject batched minimap drawing"));
	}


	// Fixed North Harbor bridge lighting
	// It worked in 5.5, and broke presumably with an upgrade to D3D7
	try {
		using namespace BridgeLightingFix;

		auto init_primitive_data = get_pattern("E8 ? ? ? ? 83 C4 ? 3B C7 74 ? 4B");
		auto render_lights = pattern("8B 11 50 68 ? ? ? ? 6A ? 51 FF 52 ? 5F B8").get_one();

		InterceptCall(init_primitive_data, orgOperatorNew, operatorNew_InitializePSize);

		// 'move' data 2 bytes back, so we have space for a direct call
		// push eax \ push D3DFVF_LVERTEX \ push D3DPT_POINTLIST \ push g_pDirect3DDevice
		Patch(render_lights.get<void>(0), { 0x50, 0x68, 0xE2, 0x01, 0x00, 0x00, 0x6A, 0x01, 0x51 });
		InjectHook(render_lights.get<void>(9), DrawBridgeLightingHook, HookType::Call);

		Logger::Log(TEXT("Bridge lighting fix injected"));
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to inject bridge lighting fix"));
	}


	// Fix horizontal raindrops at over 50 FPS
	try
	{
		using namespace RaindropsFix;

		auto ftol_raindrop = get_pattern("E8 ? ? ? ? 8B F8 E8 ? ? ? ? 25");

		InjectHook(ftol_raindrop, ftol_PreserveFracHook);

		Logger::Log(TEXT("Horizontal raindrops fix injected"));
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to inject horizontal raindrops fix"));
	}

	//Get Viewer object instance pointer
	try
	{
		m_pViewerPtr = *(int**)hook::txn::pattern("A3 ? ? ? ? 8B B0 ? ? ? ?").get_first(1);
	}
	TXN_CATCH();

	Logger::Log(TEXT("Initialization of common hooks/facades finished"));

	if (m_dwGameVersion > 60) {
		D2GIHookInjector::InjectScreenshotsPatch();

		if (options.m_bEnableUIHooks && bHasMenuGraphics && bHasBlockObserver) {
			D2GIHookInjector::InjectInterfacePatch(options.m_bEnableMirrorsHooks);
		}
	} else {
		Logger::Log(TEXT("Interface and screenshot hooks don't support this version of the game."));
	}

	// Texture names injection
#ifdef _DEBUG
	if (bHasTextureFacade) try {
		using namespace TextureNames;

		std::array<void*, 3> texture_init = {
			get_pattern("E8 ? ? ? ? 8B 45 ? 46 3B F0 7C ? 8B 0D"),
			get_pattern("E8 ? ? ? ? 85 C0 75 ? 8B 4C 24 ? 51"),
			get_pattern("E8 ? ? ? ? 8B 45 ? 46 3B F0 7C ? 68"),
		};
		HookEach_SetName(texture_init, InterceptCall);

		Logger::Log(TEXT("Texture names injection applied"));
	} catch (const hook::txn_exception&) {
		Logger::Log(TEXT("Failed to apply texture names injection"));
	}
#endif
}
