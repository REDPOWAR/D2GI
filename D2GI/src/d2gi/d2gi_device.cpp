
#include "../common.h"
#include "../logger.h"

#include "d2gi_device.h"
#include "d2gi_surface.h"
#include "d2gi_enums.h"
#include "d2gi.h"


using namespace D3D7;


D2GIDevice::D2GIDevice(D2GI* pD2GI) : DeviceProxy(), D2GIBase(pD2GI)
{

}


D2GIDevice::~D2GIDevice()
{

}


HRESULT D2GIDevice::SetTexture(DWORD i, LPDIRECTDRAWSURFACE7 pTex)
{
	if (pTex != NULL && ((D2GISurface*)pTex)->GetType() != ST_TEXTURE)
	{
		Logger::Warning(TEXT("Setting invalid surface as texture"));
		return DDERR_GENERIC;
	}

	m_pD2GI->OnTextureSet(i, (pTex == NULL) ? NULL : (D2GITexture*)pTex);
	return DD_OK;
}


HRESULT D2GIDevice::EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK pCallback, LPVOID pArg)
{
	INT i;

	for (i = 0; i < (INT)g_uTextureFormatsCount; i++)
	{
		if (!pCallback(g_asTextureFormats + i, pArg))
			break;
	}

	return DD_OK;
}


HRESULT D2GIDevice::SetViewport(D3D7::LPD3DVIEWPORT7 pVP)
{
	m_pD2GI->OnViewportSet(pVP);
	return DD_OK;
}


HRESULT D2GIDevice::Clear(DWORD dwCount, LPD3DRECT lpRects, DWORD dwFlags, D3DCOLOR col, D3DVALUE z, DWORD dwStencil)
{
	m_pD2GI->OnClear(dwCount, lpRects, dwFlags, col, z, dwStencil);
	return DD_OK;
}


HRESULT D2GIDevice::LightEnable(DWORD i, BOOL bEnable)
{
	m_pD2GI->OnLightEnable(i, bEnable);
	return DD_OK;
}


HRESULT D2GIDevice::BeginScene()
{
	m_pD2GI->OnSceneBegin();
	return DD_OK;
}


HRESULT D2GIDevice::EndScene()
{
	m_pD2GI->OnSceneEnd();
	return DD_OK;
}


HRESULT D2GIDevice::SetRenderState(D3D7::D3DRENDERSTATETYPE eState, DWORD dwValue)
{
	m_pD2GI->OnRenderStateSet(eState, dwValue);
	return DD_OK;
}


HRESULT D2GIDevice::SetTextureStageState(DWORD i, D3D7::D3DTEXTURESTAGESTATETYPE eState, DWORD dwValue)
{
	m_pD2GI->OnTextureStageSet(i, eState, dwValue);
	return DD_OK;
}


HRESULT D2GIDevice::ValidateDevice(LPDWORD pdw)
{
	if (m_pD2GI->OnDeviceValidate(pdw))
		return DD_OK;

	Logger::Log(TEXT("Device validation failed"));
	return DDERR_GENERIC;
}


HRESULT D2GIDevice::SetTransform(D3D7::D3DTRANSFORMSTATETYPE eType, D3D7::LPD3DMATRIX pMatrix)
{
	m_pD2GI->OnTransformSet(eType, pMatrix);
	return DD_OK;
}


HRESULT D2GIDevice::SetLight(DWORD i, D3D7::LPD3DLIGHT7 pLight)
{
	m_pD2GI->OnLightSet(i, pLight);
	return DD_OK;
}


HRESULT D2GIDevice::SetMaterial(D3D7::LPD3DMATERIAL7 pMaterial)
{
	m_pD2GI->OnMaterialSet(pMaterial);
	return DD_OK;
}


HRESULT D2GIDevice::SetClipStatus(D3D7::D3DCLIPSTATUS* pStatus)
{
	m_pD2GI->OnClipStatusSet(pStatus);
	return DD_OK;
}


HRESULT D2GIDevice::DrawIndexedPrimitiveStrided(
	D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData,
	DWORD dwCount, LPWORD pIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	m_pD2GI->OnIndexedPrimitiveStridedDraw(pt, dwFVF, pData, dwCount, pIdx, dwIdxCount, dwFlags);
	return DD_OK;
}


HRESULT D2GIDevice::DrawPrimitiveStrided(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPD3DDRAWPRIMITIVESTRIDEDDATA pData, DWORD dwCount, DWORD dwFlags)
{
	m_pD2GI->OnPrimitiveStridedDraw(pt, dwFVF, pData, dwCount, dwFlags);
	return DD_OK;
}



HRESULT D2GIDevice::DrawPrimitive(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID pVerts, DWORD dwVertCount, DWORD dwFlags)
{
	m_pD2GI->OnPrimitiveDraw(pt, dwFVF, pVerts, dwVertCount, dwFlags);
	return DD_OK;
}


HRESULT D2GIDevice::DrawIndexedPrimitive(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID pVerts, DWORD dwVertCount, LPWORD pIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	m_pD2GI->OnIndexedPrimitiveDraw(pt, dwFVF, pVerts, dwVertCount, pIdx, dwIdxCount, dwFlags);
	return DD_OK;
}


HRESULT D2GIDevice::GetRenderState(D3D7::D3DRENDERSTATETYPE eState, LPDWORD pV)
{
	if (m_pD2GI->OnRenderStateGet(eState, pV))
		return DD_OK;

	Logger::Warning(TEXT("Requested render state %i (not implemented)"), eState);
	return DDERR_GENERIC;
}
