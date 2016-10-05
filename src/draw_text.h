#pragma once

#include "spherefonts/font.h"

#include "lantern_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Lantern_FontContext;
struct Lantern_FontContext *Lantern_CreateFontContext();
void Lantern_DestroyFontContext(struct Lantern_FontContext *);
/*
void Lantern_InitFontContext(struct Lantern_FontContext * LANTERN_RESTRICT to,
    const struct Sphere_Font * LANTERN_RESTRICT font);
const struct Sphere_Font *LANTERN_PURE(Lantern_GetFontContextFont());
*/
void Lantern_AddTextToFontContext(
    struct Lantern_FontContext *LANTERN_RESTRICT ctx,
    const char *LANTERN_RESTRICT text, 
    unsigned x, unsigned y);

void Lantern_DrawFontContext(const struct Lantern_FontContext *ctx);
void Lantern_ClearFontContext(struct Lantern_FontContext *ctx);

#ifdef __cplusplus
}
#endif
