#pragma once

#include "lantern_gl.h"
#include "lantern_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

void Lantern_InitPrimitivesSubsystem();

struct Lantern_Primitive;
unsigned LANTERN_CONST(Lantern_PrimitiveSize());
void Lantern_InitPrimitive(struct Lantern_Primitive *primitive);
void Lantern_DestroyPrimitive(struct Lantern_Primitive *primitive);

void Lantern_CreateRectangle(struct Lantern_Primitive *primitive, unsigned w, unsigned h, LX_Texture tex);
void Lantern_DrawPrimitive(const struct Lantern_Primitive *primitive);

#ifdef __cplusplus
}
#endif
