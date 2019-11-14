#pragma once

#include "common.h"
#include "d3d9.h"


struct MAT3X4
{
	union
	{
		struct
		{
			FLOAT _11, _12, _13;
			FLOAT _21, _22, _23;
			FLOAT _31, _32, _33;
			FLOAT _41, _42, _43;
		};
		FLOAT m[4][3];
	};

	MAT3X4() {};
	MAT3X4(CONST D3D9::D3DXMATRIX&);
	operator D3D9::D3DXMATRIX();
	MAT3X4 Transpose();
};
