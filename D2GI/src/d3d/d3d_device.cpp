
#include "../common.h"

#include "d3d_device.h"


DeviceProxy::DeviceProxy(IDirect3DDevice7* pOriginal) : m_pOriginal(pOriginal)
{

}


DeviceProxy::~DeviceProxy()
{
	Debug(TEXT("Device destroyed"));
}


HRESULT DeviceProxy::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	return m_pOriginal->QueryInterface(riid, ppvObj);
}


ULONG DeviceProxy::AddRef()
{
	return m_pOriginal->AddRef();
}


ULONG DeviceProxy::Release()
{
	ULONG uRes = m_pOriginal->Release();

	if (uRes == 0)
		delete this;

	return uRes;
}


HRESULT DeviceProxy::GetCaps(LPD3DDEVICEDESC7 lpDesc)
{
	return m_pOriginal->GetCaps(lpDesc);
}


HRESULT DeviceProxy::EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK lpCallback, LPVOID lpArg)
{
	Debug(TEXT("Enumerating texture formats"));
	return m_pOriginal->EnumTextureFormats(lpCallback, lpArg);
}


HRESULT DeviceProxy::BeginScene()
{
	return m_pOriginal->BeginScene();
}


HRESULT DeviceProxy::EndScene()
{
	return m_pOriginal->EndScene();
}


HRESULT DeviceProxy::GetDirect3D(LPDIRECT3D7* lpD3D)
{
	return m_pOriginal->GetDirect3D(lpD3D);
}


HRESULT DeviceProxy::SetRenderTarget(LPDIRECTDRAWSURFACE7 lpSurf, DWORD dwFlags)
{
	return m_pOriginal->SetRenderTarget(lpSurf, dwFlags);
}


HRESULT DeviceProxy::GetRenderTarget(LPDIRECTDRAWSURFACE7* lpSurf)
{
	return m_pOriginal->GetRenderTarget(lpSurf);
}


HRESULT DeviceProxy::Clear(DWORD dwCount, LPD3DRECT lpRects, DWORD dwFlags, D3DCOLOR col, D3DVALUE z, DWORD dwStencil)
{
	return m_pOriginal->Clear(dwCount, lpRects, dwFlags, col, z, dwStencil);
}


HRESULT DeviceProxy::SetTransform(D3DTRANSFORMSTATETYPE state, LPD3DMATRIX lpMatrix)
{
	return m_pOriginal->SetTransform(state, lpMatrix);
}


HRESULT DeviceProxy::GetTransform(D3DTRANSFORMSTATETYPE state, LPD3DMATRIX lpMatrix)
{
	return m_pOriginal->GetTransform(state, lpMatrix);
}


HRESULT DeviceProxy::SetViewport(LPD3DVIEWPORT7 lpViewport)
{
	Debug(TEXT("Setting viewport: %i:%i %ix%i %f - %f"), 
		lpViewport->dwX, lpViewport->dwY, lpViewport->dwWidth, lpViewport->dwHeight, 
		lpViewport->dvMinZ, lpViewport->dvMaxZ);
	return m_pOriginal->SetViewport(lpViewport);
}


HRESULT DeviceProxy::MultiplyTransform(D3DTRANSFORMSTATETYPE state, LPD3DMATRIX lpMatrix)
{
	return m_pOriginal->MultiplyTransform(state, lpMatrix);
}


HRESULT DeviceProxy::GetViewport(LPD3DVIEWPORT7 lpViewport)
{
	return m_pOriginal->GetViewport(lpViewport);
}


HRESULT DeviceProxy::SetMaterial(LPD3DMATERIAL7 lpMaterial)
{
	return m_pOriginal->SetMaterial(lpMaterial);
}


HRESULT DeviceProxy::GetMaterial(LPD3DMATERIAL7 lpMaterial)
{
	return m_pOriginal->GetMaterial(lpMaterial);
}


HRESULT DeviceProxy::SetLight(DWORD i, LPD3DLIGHT7 lpLight)
{
	return m_pOriginal->SetLight(i, lpLight);
}


HRESULT DeviceProxy::GetLight(DWORD i, LPD3DLIGHT7 lpLight)
{
	return m_pOriginal->GetLight(i, lpLight);
}


HRESULT DeviceProxy::SetRenderState(D3DRENDERSTATETYPE state, DWORD v)
{
	return m_pOriginal->SetRenderState(state, v);
}


HRESULT DeviceProxy::GetRenderState(D3DRENDERSTATETYPE state, LPDWORD lpv)
{
	return m_pOriginal->GetRenderState(state, lpv);
}


HRESULT DeviceProxy::BeginStateBlock()
{
	return m_pOriginal->BeginStateBlock();
}


HRESULT DeviceProxy::EndStateBlock(LPDWORD lpdw)
{
	return m_pOriginal->EndStateBlock(lpdw);
}


HRESULT DeviceProxy::PreLoad(LPDIRECTDRAWSURFACE7 lpSurf)
{
	return m_pOriginal->PreLoad(lpSurf);
}


HRESULT DeviceProxy::DrawPrimitive(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID lpVerts, DWORD dwVertCount, DWORD dwFlags)
{
	return m_pOriginal->DrawPrimitive(pt, dwFVF, lpVerts, dwVertCount, dwFlags);
}


HRESULT DeviceProxy::DrawIndexedPrimitive(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID lpVerts, DWORD dwVertCount, LPWORD lpIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	return m_pOriginal->DrawIndexedPrimitive(pt, dwFVF, lpVerts, dwVertCount, lpIdx, dwIdxCount, dwFlags);
}


HRESULT DeviceProxy::SetClipStatus(LPD3DCLIPSTATUS lpStatus)
{
	return m_pOriginal->SetClipStatus(lpStatus);
}


HRESULT DeviceProxy::GetClipStatus(LPD3DCLIPSTATUS lpStatus)
{
	return m_pOriginal->GetClipStatus(lpStatus);
}


HRESULT DeviceProxy::DrawPrimitiveStrided(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPD3DDRAWPRIMITIVESTRIDEDDATA lpData, DWORD dwCount, DWORD dwFlags)
{
	return m_pOriginal->DrawPrimitiveStrided(pt, dwFVF, lpData, dwCount, dwFlags);
}


HRESULT DeviceProxy::DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE pt, DWORD dwFVF, LPD3DDRAWPRIMITIVESTRIDEDDATA lpData, DWORD dwCount, LPWORD lpIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	return m_pOriginal->DrawIndexedPrimitiveStrided(pt, dwFVF, lpData, dwCount, lpIdx, dwIdxCount, dwFlags);
}


HRESULT DeviceProxy::DrawPrimitiveVB(D3DPRIMITIVETYPE pt, LPDIRECT3DVERTEXBUFFER7 lpVB, DWORD dwStartVert, DWORD dwCount, DWORD dwFlags)
{
	return m_pOriginal->DrawPrimitiveVB(pt, lpVB, dwStartVert, dwCount, dwFlags);
}


HRESULT DeviceProxy::DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE pt, LPDIRECT3DVERTEXBUFFER7 lpVB, DWORD dwStartVert, DWORD dwCount, LPWORD lpIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	return m_pOriginal->DrawIndexedPrimitiveVB(pt, lpVB, dwStartVert, dwCount, lpIdx, dwIdxCount, dwFlags);
}


HRESULT DeviceProxy::ComputeSphereVisibility(LPD3DVECTOR lpCenters, LPD3DVALUE lpRads, DWORD dwCount, DWORD dwFlags, LPDWORD lpResults)
{
	return m_pOriginal->ComputeSphereVisibility(lpCenters, lpRads, dwCount, dwFlags, lpResults);
}


HRESULT DeviceProxy::GetTexture(DWORD i, LPDIRECTDRAWSURFACE7* lpTex)
{
	return m_pOriginal->GetTexture(i, lpTex);
}


HRESULT DeviceProxy::SetTexture(DWORD i, LPDIRECTDRAWSURFACE7 lpTex)
{
	return m_pOriginal->SetTexture(i, lpTex);
}


HRESULT DeviceProxy::GetTextureStageState(DWORD i, D3DTEXTURESTAGESTATETYPE state, LPDWORD lpdw)
{
	return m_pOriginal->GetTextureStageState(i, state, lpdw);
}


HRESULT DeviceProxy::SetTextureStageState(DWORD i, D3DTEXTURESTAGESTATETYPE state, DWORD dw)
{
	return m_pOriginal->SetTextureStageState(i, state, dw);
}


HRESULT DeviceProxy::ValidateDevice(LPDWORD lpdw)
{
	Debug(TEXT("Validating device"));
	return m_pOriginal->ValidateDevice(lpdw);
}


HRESULT DeviceProxy::ApplyStateBlock(DWORD dwHandle)
{
	return m_pOriginal->ApplyStateBlock(dwHandle);
}


HRESULT DeviceProxy::CaptureStateBlock(DWORD dwHandle)
{
	return m_pOriginal->CaptureStateBlock(dwHandle);
}


HRESULT DeviceProxy::DeleteStateBlock(DWORD dwHandle)
{
	return m_pOriginal->DeleteStateBlock(dwHandle);
}


HRESULT DeviceProxy::CreateStateBlock(D3DSTATEBLOCKTYPE type, LPDWORD lpHandle)
{
	return m_pOriginal->CreateStateBlock(type, lpHandle);
}


HRESULT DeviceProxy::Load(LPDIRECTDRAWSURFACE7 lpDestSurf, LPPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcSurf, LPRECT lpSrcRect, DWORD dwFlags)
{
	return m_pOriginal->Load(lpDestSurf, lpDestPoint, lpSrcSurf, lpSrcRect, dwFlags);
}


HRESULT DeviceProxy::LightEnable(DWORD i, BOOL bEnable)
{
	return m_pOriginal->LightEnable(i, bEnable);
}


HRESULT DeviceProxy::GetLightEnable(DWORD i, BOOL* lpEnable)
{
	return m_pOriginal->GetLightEnable(i, lpEnable);
}


HRESULT DeviceProxy::SetClipPlane(DWORD i, D3DVALUE* lpV)
{
	return m_pOriginal->SetClipPlane(i, lpV);
}


HRESULT DeviceProxy::GetClipPlane(DWORD i, D3DVALUE* lpV)
{
	return m_pOriginal->GetClipPlane(i, lpV);
}


HRESULT DeviceProxy::GetInfo(DWORD i, LPVOID lpOut, DWORD dwSize)
{
	return m_pOriginal->GetInfo(i, lpOut, dwSize);
}
