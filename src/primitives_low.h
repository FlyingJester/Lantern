#pragma once

#include "lantern_gl.h"

struct Lantern_Primitive{
	LX_Buffer vbo;
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
    data[1] = data[3] = (Y);\
    data[0] = data[6] = (X);\
	data[2] = data[4] = (X)+(W);\
	data[5] = data[7] = (Y)+(H)
