
#include "d2gi_frustum.h"


using namespace D3D9;


D2GIFrustum::D2GIFrustum(D2GI* pD2GI) : D2GIBase(pD2GI)
{
	m_vPos = D3DXVECTOR3(0.0, 0.0, 0.0);
	m_vDir = D3DXVECTOR3(1.0f, 0.0, 0.0);
	m_vRight = D3DXVECTOR3(0.0, -1.0f, 0.0);
	m_vUp = D3DXVECTOR3(0.0, 0.0, 1.0f);
	m_fZNear = 0.0;
	m_fZFar = 1.0f;
	m_fFOV = D3DX_PI * 0.25f;
	m_fAspectRatio = 1.0f;
}


D2GIFrustum::~D2GIFrustum()
{

}


VOID D2GIFrustum::SetPerspective(FLOAT fZNear, FLOAT fZFar, FLOAT fFOV, FLOAT fAspectRatio)
{
	m_fZNear = fZNear;
	m_fZFar = fZFar;
	m_fFOV = fFOV;
	m_fAspectRatio = fAspectRatio;

	UpdatePlanes();
}


VOID D2GIFrustum::SetView(D3DXVECTOR3* pPos, D3DXVECTOR3* pDir, D3DXVECTOR3* pRight, D3DXVECTOR3* pUp)
{
	m_vPos = *pPos;
	D3DXVec3Normalize(&m_vDir, pDir);
	D3DXVec3Normalize(&m_vRight, pRight);
	D3DXVec3Normalize(&m_vUp, pUp);

	UpdatePlanes();
}


BOOL D2GIFrustum::TestSphereClipping(SPHERE* pSphere, UINT* pClippingFlags)
{
	static UINT auCF[] =
	{
		CF_NEAR_INTERSECTION, CF_FAR_INTERSECTION, CF_LEFT_INTERSECTION,
		CF_RIGHT_INTERSECTION, CF_BOTTOM_INTERSECTION, CF_TOP_INTERSECTION
	};

	INT i;

	*pClippingFlags = 0;
	for (i = 0; i < 6; i++)
	{
		FLOAT fD = D3DXPlaneDotCoord(m_asPlanes + i, (D3DXVECTOR3*)pSphere);

		if (fD < -pSphere->fRadius)
			return FALSE;
		else if (fD < pSphere->fRadius)
			*pClippingFlags |= auCF[i];
	}

	return TRUE;
}


VOID D2GIFrustum::UpdatePlanes()
{
	D3DXVECTOR3 vNearCenter = m_vPos + m_vDir * m_fZNear;
	D3DXVECTOR3 vFarCenter = m_vPos + m_vDir * m_fZFar;

	D3DXPlaneFromPointNormal(m_asPlanes + 0, &vNearCenter, &m_vDir);
	D3DXPlaneFromPointNormal(m_asPlanes + 1, &vFarCenter, &(-m_vDir));

	D3DXVECTOR3 vNearLT, vNearLB, vNearRT, vNearRB;
	FLOAT fNearHalfWidth, fNearHalfHeight;

	fNearHalfHeight = tanf(m_fFOV * 0.5f) * m_fZNear;
	fNearHalfWidth = fNearHalfHeight * m_fAspectRatio;
	vNearLT = vNearCenter - m_vRight * fNearHalfWidth + m_vUp * fNearHalfHeight;
	vNearLB = vNearCenter - m_vRight * fNearHalfWidth - m_vUp * fNearHalfHeight;
	vNearRT = vNearCenter + m_vRight * fNearHalfWidth + m_vUp * fNearHalfHeight;
	vNearRB = vNearCenter + m_vRight * fNearHalfWidth - m_vUp * fNearHalfHeight;

	D3DXVECTOR3 vFarLT, vFarLB, vFarRT, vFarRB;
	FLOAT fFarHalfWidth, fFarHalfHeight;

	fFarHalfHeight = tanf(m_fFOV * 0.5f) * m_fZFar;
	fFarHalfWidth = fFarHalfHeight * m_fAspectRatio;
	vFarLT = vFarCenter - m_vRight * fFarHalfWidth + m_vUp * fFarHalfHeight;
	vFarLB = vFarCenter - m_vRight * fFarHalfWidth - m_vUp * fFarHalfHeight;
	vFarRT = vFarCenter + m_vRight * fFarHalfWidth + m_vUp * fFarHalfHeight;
	vFarRB = vFarCenter + m_vRight * fFarHalfWidth - m_vUp * fFarHalfHeight;

	D3DXPlaneFromPoints(m_asPlanes + 2, &vFarLT, &vNearLT, &vNearLB);
	D3DXPlaneFromPoints(m_asPlanes + 3, &vFarRT, &vFarRB, &vNearRB);
	D3DXPlaneFromPoints(m_asPlanes + 4, &vFarRB, &vFarLB, &vNearLB);
	D3DXPlaneFromPoints(m_asPlanes + 5, &vFarRT, &vNearRT, &vNearLT);

	m_avPoints[0] = vNearLB;
	m_avPoints[1] = vNearLT;
	m_avPoints[2] = vNearRB;
	m_avPoints[3] = vNearRT;
	m_avPoints[4] = vFarLB;
	m_avPoints[5] = vFarLT;
	m_avPoints[6] = vFarRB;
	m_avPoints[7] = vFarRT;
}
