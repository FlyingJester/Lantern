#pragma once
#include "primitives.h"
#include "servers/textures.hpp"

namespace Lantern {

class Primitive {
    Lantern_Primitive *const p;
public:
    
    Primitive(void *data);
    ~Primitive(){ Lantern_DestroyPrimitive(p); }
    inline void draw(unsigned x, unsigned y) const {
        Lantern_DrawPrimitive(p, x, y);
    }
    
    void UVRectangle(float u0, float v0, float u1, float v1,
        unsigned w, unsigned h, const Image &img);
};

#define LANTERN_DECLPRIMTIVE(NAME)\
    Lantern::Primitive NAME(alloca(Lantern_PrimitiveSize()));
#define LANTERN_CREATEPRIMTIVE(NAME)\
    Lantern::Primitive NAME(alloca(Lantern_PrimitiveSize()));

} // namespace Lantern