#pragma once

#include "SDL_endian.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define A_VALUE(val) (val)
#define R_VALUE(val) ((val) << 8)
#define G_VALUE(val) ((val) << 16)
#define B_VALUE(val) ((val) << 24)
#else
#define A_VALUE(val) ((val) << 24)
#define R_VALUE(val) ((val) << 16)
#define G_VALUE(val) ((val) << 8)
#define B_VALUE(val) (val)
#endif