
#include "../common/common.h"

#include "d3d_device.h"


using namespace D3D7;


DeviceProxy::DeviceProxy() 
{

}


DeviceProxy::~DeviceProxy()
{
}



HRESULT DeviceProxy::GetCaps(LPD3DDEVICEDESC7 lpDesc)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK lpCallback, LPVOID lpArg)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::BeginScene()
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::EndScene()
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetDirect3D(LPDIRECT3D7* lpD3D)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetRenderTarget(LPDIRECTDRAWSURFACE7 lpSurf, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetRenderTarget(LPDIRECTDRAWSURFACE7* lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::Clear(DWORD dwCount, LPD3DRECT lpRects, DWORD dwFlags, D3DCOLOR col, D3DVALUE z, DWORD dwStencil)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetTransform(D3DTRANSFORMSTATETYPE state, LPD3DMATRIX lpMatrix)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetTransform(D3DTRANSFORMSTATETYPE state, LPD3DMATRIX lpMatrix)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetViewport(LPD3DVIEWPORT7 lpViewport)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::MultiplyTransform(D3DTRANSFORMSTATETYPE state, LPD3DMATRIX lpMatrix)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetViewport(LPD3DVIEWPORT7 lpViewport)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetMaterial(LPD3DMATERIAL7 lpMaterial)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetMaterial(LPD3DMATERIAL7 lpMaterial)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetLight(DWORD i, LPD3DLIGHT7 lpLight)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetLight(DWORD i, LPD3DLIGHT7 lpLight)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetRenderState(D3DRENDERSTATETYPE state, DWORD v)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetRenderState(D3DRENDERSTATETYPE state, LPDWORD lpv)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::BeginStateBlock()
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::EndStateBlock(LPDWORD lpdw)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::PreLoad(LPDIRECTDRAWSURFACE7 lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::DrawPrimitive(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID lpVerts, DWORD dwVertCount, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::DrawIndexedPrimitive(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID lpVerts, DWORD dwVertCount, LPWORD lpIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetClipStatus(LPD3DCLIPSTATUS lpStatus)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetClipStatus(LPD3DCLIPSTATUS lpStatus)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::DrawPrimitiveStrided(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPD3DDRAWPRIMITIVESTRIDEDDATA lpData, DWORD dwCount, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPD3DDRAWPRIMITIVESTRIDEDDATA lpData, DWORD dwCount, LPWORD lpIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::DrawPrimitiveVB(D3DPRIMITIVETYPE pt, LPDIRECT3DVERTEXBUFFER7 lpVB, DWORD dwStartVert, DWORD dwCount, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE pt, LPDIRECT3DVERTEXBUFFER7 lpVB, DWORD dwStartVert, DWORD dwCount, LPWORD lpIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::ComputeSphereVisibility(LPD3DVECTOR lpCenters, LPD3DVALUE lpRads, DWORD dwCount, DWORD dwFlags, LPDWORD lpResults)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetTexture(DWORD i, LPDIRECTDRAWSURFACE7* lpTex)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetTexture(DWORD i, LPDIRECTDRAWSURFACE7 lpTex)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetTextureStageState(DWORD i, D3DTEXTURESTAGESTATETYPE state, LPDWORD lpdw)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetTextureStageState(DWORD i, D3DTEXTURESTAGESTATETYPE state, DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::ValidateDevice(LPDWORD lpdw)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::ApplyStateBlock(DWORD dwHandle)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::CaptureStateBlock(DWORD dwHandle)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::DeleteStateBlock(DWORD dwHandle)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::CreateStateBlock(D3DSTATEBLOCKTYPE type, LPDWORD lpHandle)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::Load(LPDIRECTDRAWSURFACE7 lpDestSurf, LPPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcSurf, LPRECT lpSrcRect, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::LightEnable(DWORD i, BOOL bEnable)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetLightEnable(DWORD i, BOOL* lpEnable)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::SetClipPlane(DWORD i, D3DVALUE* lpV)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetClipPlane(DWORD i, D3DVALUE* lpV)
{
	return DDERR_GENERIC;
}


HRESULT DeviceProxy::GetInfo(DWORD i, LPVOID lpOut, DWORD dwSize)
{
	return DDERR_GENERIC;
}
