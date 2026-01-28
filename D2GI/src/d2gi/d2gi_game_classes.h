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
	{}

public:
	FACADE_STABLE_MEMBER(const char*, m_path, 4); // 0x4 in 8.2 - first class member so assume it doesn't change
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
