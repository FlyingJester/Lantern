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

class WindowStyle {
    Lantern_Primitive *const p;
    unsigned m_w, m_h;
public:

    inline unsigned w() const { return m_w; }
    inline unsigned h() const { return m_h; }
    
    WindowStyle(TextureServer &texture_server, unsigned w, unsigned h);
    ~WindowStyle();
    
    void draw(unsigned x, unsigned y) const;
};

#define LANTERN_DECLPRIMTIVE(NAME)\
    Lantern::Primitive NAME(alloca(Lantern_PrimitiveSize()));
#define LANTERN_CREATEPRIMTIVE(NAME)\
    Lantern::Primitive NAME(alloca(Lantern_PrimitiveSize()));

} // namespace Lantern