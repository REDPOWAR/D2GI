#pragma once

#include "common.h"


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
};
