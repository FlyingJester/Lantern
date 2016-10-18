#include "primitives_obj.hpp"

namespace Lantern {

Primitive::Primitive(void *data)
  : p((Lantern_Primitive *)data){
    Lantern_InitPrimitive(p);
}

void Primitive::UVRectangle(float u0, float v0, float u1, float v1,
    unsigned w, unsigned h, const Image &img){
    Lantern_CreateUVRectangle(u0, v0, u1, v1, p, w, h, img.texture);
}

} // namespace Lantern