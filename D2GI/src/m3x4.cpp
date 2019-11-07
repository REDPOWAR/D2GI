
#include "m3x4.h"


MAT3X4::MAT3X4(CONST D3D9::D3DXMATRIX& m4x4)
{
	_11 = m4x4._11;
	_13 = m4x4._12;
	_12 = m4x4._13;
	_21 = m4x4._21;
	_23 = m4x4._22;
	_22 = m4x4._23;
	_31 = m4x4._31;
	_33 = m4x4._32;
	_32 = m4x4._33;
	_41 = m4x4._41;
	_43 = m4x4._42;
	_42 = m4x4._43;
}


MAT3X4::operator D3D9::D3DXMATRIX()
{
	D3D9::D3DXMATRIX mResult;

	D3D9::D3DXMatrixIdentity(&mResult);

	mResult._11 = _11;
	mResult._12 = _13;
	mResult._13 = _12;
	mResult._21 = _21;
	mResult._22 = _23;
	mResult._23 = _22;
	mResult._31 = _31;
	mResult._32 = _33;
	mResult._33 = _32;
	mResult._41 = _41;
	mResult._42 = _43;
	mResult._43 = _42;

	return mResult;
}
