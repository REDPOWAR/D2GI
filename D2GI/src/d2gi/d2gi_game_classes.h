#pragma once

#include "Utils/Facade.hpp"

class ResMaterial;
class ResTexture;

class ResMaterialFacade
{
public:
	ResMaterialFacade(ResMaterial* obj)
		: FACADE_INIT_MEMBER(obj, m_texture)
	{}

public:
	FACADE_MEMBER(ResTexture*, m_texture); // 0x4C in 8.2
};

class ResTextureFacade
{
public:
	ResTextureFacade(ResTexture* obj)
		: FACADE_INIT_MEMBER(obj, m_path)
		, FACADE_INIT_MEMBER(obj, m_d3dSurface)
	{}

public:
	FACADE_STABLE_MEMBER(const char*, m_path, 4); // 0x4 in 8.2 - first class member so assume it doesn't change
	FACADE_MEMBER(D2GISurface*, m_d3dSurface); // 0x48 in 8.2
};

class D3DRenderData;
class GameModule;

class D3DRenderDataFacade
{
public:
	D3DRenderDataFacade(D3DRenderData* obj)
		: FACADE_INIT_MEMBER(obj, m_currentGameModule)
	{}

public:
	FACADE_MEMBER(GameModule*, m_currentGameModule); // 0x21B0 in 8.2
};

class GameModuleFacade
{
public:
	GameModuleFacade(GameModule* obj)
		:FACADE_INIT_MEMBER(obj, m_name)
	{}

public:
	FACADE_MEMBER(const char*, m_name); // 0x3C in 8.2
};

class MenuGraphics;

class MenuGraphicsFacade
{
public:
	MenuGraphicsFacade(MenuGraphics* obj)
		: FACADE_INIT_MEMBER(obj, m_videoIni)
	{}

public:
	FACADE_MEMBER(uint32_t, m_videoIni); // 0x190 in 7.3, 0x198 in 8.2
};

// Currently unused, but let's leave it as documentation
/*
class CMenu;
class Sprite;

class SpriteFacade
{
public:
	SpriteFacade(Sprite* obj)
		: FACADE_INIT_MEMBER(obj, m_rect)
	{}

public:
	FACADE_MEMBER(RECT, m_rect); // 0x28 in 7.3, 0x2C in 8.x/KotR 1.3
};

class CMenuFacade
{
public:
	CMenuFacade(CMenu* obj)
		: FACADE_INIT_MEMBER(obj, m_sprite)
	{}

public:
	FACADE_MEMBER(Sprite*, m_sprite); // 0x38 in 7.3 and 8.2
};
*/

class CBlockObserver;

class CBlockObserverFacade
{
public:
	CBlockObserverFacade(CBlockObserver* obj)
		: FACADE_INIT_MEMBER(obj, m_fovX)
		, FACADE_INIT_MEMBER(obj, m_fovY)
	{}

public:
	FACADE_MEMBER(float, m_fovX); // 0x58
	FACADE_MEMBER(float, m_fovY); // 0x54
};

class InterfaceHookOffsets
{
public:
	int addr_panelDrawStart, //panel offsets setup code address
		addr_cmp1204,        //only for 6.6 - 8.2
		addr_cmp800,
		addr_pagerX,         //pager position X
		addr_pagerY,         //pager position Y
		addr_panelX,         //panel texture X offset
		addr_textX,          //panel labels X offset
		addr_resX,           //default resolution 1024x768 offsets
		addr_resY,
		addr_mapOffsetX;     //side minimap offset X
	double m_dMapSideOffset;
};