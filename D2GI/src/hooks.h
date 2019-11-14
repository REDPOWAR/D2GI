#pragma once

#include "common.h"
#include "m3x4.h"


VOID InjectHooks();

INT CallOriginalSetupTransforms(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj);
