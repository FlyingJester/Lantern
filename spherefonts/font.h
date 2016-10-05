#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

	struct Sphere_Glyph {
		unsigned x, w, h;
	};

    struct Sphere_Font {
		unsigned texture; /* An OpenGL texture */
		struct Sphere_Glyph glyphs[0xFF];
		/* master.data can be freed to release some memory. */
        struct { unsigned w, h; uint32_t *data; } master;
    };
    
    bool LoadFontMem(struct Sphere_Font *to, const uint32_t *mem, const uint64_t size);
    
    uint64_t StringWidth(const struct Sphere_Font *font, const char *str, uint16_t n);
    const struct Sphere_Font *GetSystemFont();
    const struct Sphere_Glyph *GetBoundedGlyph(const struct Sphere_Font *font, unsigned i);

#ifdef __cplusplus
}
#endif
