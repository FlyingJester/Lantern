#pragma once

#include "lantern_gl.h"

struct Lantern_Primitive{
	LX_Buffer vbo, tex_coord;
	unsigned length;
	enum LX_DrawType type;
	LX_Texture texture;
};

/* Same as DrawPrimitive, but does not update textures. */
#ifdef __cplusplus
extern "C"
#endif
void Lantern_DrawPrimitiveLow(LX_Buffer vertex, LX_Buffer tex_coord, unsigned length, enum LX_DrawType type);

#define RECTANGLE_COORDS(NAME, X, Y, W, H)\
	float NAME[8];\
    NAME[1] = NAME[3] = (Y);\
    NAME[0] = NAME[6] = (X);\
	NAME[2] = NAME[4] = (X)+(W);\
	NAME[5] = NAME[7] = (Y)+(H)
