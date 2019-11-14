#pragma once

#include "../common.h"
#include "d2gi_common.h"


enum CLIPPING_FLAGS
{
	CF_NEAR_INTERSECTION   = 0x00000001,
	CF_LEFT_INTERSECTION   = 0x00000002,
	CF_TOP_INTERSECTION    = 0x00000004,
	CF_BOTTOM_INTERSECTION = 0x00000008,
	CF_RIGHT_INTERSECTION  = 0x00000010,
	CF_FAR_INTERSECTION    = 0x00000020,
};


class D2GIFrustum : public D2GIBase
{
	D3D9::D3DXVECTOR3 m_vPos, m_vDir, m_vRight, m_vUp;
	FLOAT m_fZNear, m_fZFar, m_fFOV, m_fAspectRatio;

	D3D9::D3DXPLANE m_asPlanes[6];
	D3D9::D3DXVECTOR3 m_avPoints[8];

	VOID UpdatePlanes();
public:
	D2GIFrustum(D2GI*);
	~D2GIFrustum();

	VOID SetPerspective(FLOAT fZNear, FLOAT fZFar, FLOAT fFOV, FLOAT fAspectRatio);
	VOID SetView(D3D9::D3DXVECTOR3* pPos, D3D9::D3DXVECTOR3* pDir, D3D9::D3DXVECTOR3* pRight, D3D9::D3DXVECTOR3* pUp);

	BOOL TestSphereClipping(SPHERE*, UINT*);
	D3D9::D3DXVECTOR3* GetPoints() { return m_avPoints; }
};
