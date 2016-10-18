#include "primitives_obj.hpp"
#include "primitives_low.h"

namespace Lantern {

Primitive::Primitive(void *data)
  : p((Lantern_Primitive *)data){
    Lantern_InitPrimitive(p);
}

void Primitive::UVRectangle(float u0, float v0, float u1, float v1,
    unsigned w, unsigned h, const Image &img){
    Lantern_CreateUVRectangle(u0, v0, u1, v1, p, w, h, img.texture);
}

enum WS_Element {
    eUL,
    eU,
    eUR,
    eR,
    eDR,
    eD,
    eDL,
    eL,
    eNumWSElements
};

WindowStyle::WindowStyle(TextureServer &texture_server, unsigned w, unsigned h)
  : p(new Lantern_Primitive[eNumWSElements + 3])
  , m_w(w)
  , m_h(h){
    for(int i = 0; i < static_cast<int>(eNumWSElements) + 3; i++)
        Lantern_InitPrimitive(p + i);
    
    Lantern::Image text_background_image = texture_server.load("windowstyle1.tga"),
        outline_image = texture_server.load("windowstyle.png");
    
#define ELEMENT(WHICH, U0, V0, U1, V1, W, H)\
    Lantern_CreateUVRectangle(U0, V0, U1, V1, p + static_cast<int>(WHICH), W, H, outline_image.texture)
   
#define CORNER(WHICH_, U, V) ELEMENT(WHICH_, U, V, U + 0.5f, V + 0.5f, 16, 16);
    
    CORNER(eUL, 0.0f, 0.0f);
    CORNER(eUR, 0.5f, 0.0f);
    CORNER(eDR, 0.5f, 0.5f);
    CORNER(eDL, 0.0f, 0.5f);
    
#undef CORNER
    
    const unsigned inner_h = h - 32;
    
    ELEMENT(eU, 0.48f, 0.0f, 0.49f, 0.5f, w - 32, 16);
    ELEMENT(eL, 0.0f, 0.48f, 0.5f, 0.49f, 16, inner_h);
    ELEMENT(eD, 0.51f, 0.5f, 0.52f, 1.0f, w - 32, 16);
    ELEMENT(eR, 0.5f, 0.51f, 1.0f, 0.52f, 16, inner_h);
    
#undef ELEMENT
    
    Lantern_Primitive *const bckgnd = p + static_cast<int>(eNumWSElements);
    
    const float h_repeat = 0.75 *  inner_h / static_cast<float>(text_background_image.h);
    Lantern_CreateUVRectangle(0.0f,  0.0f, 0.25f, h_repeat, bckgnd + 0, 16, inner_h, text_background_image.texture);
    Lantern_CreateUVRectangle(0.25f, 0.0f, 0.75f, h_repeat, bckgnd + 1, w - 64, inner_h, text_background_image.texture);
    Lantern_CreateUVRectangle(0.75f, 0.0f, 1.0f,  h_repeat, bckgnd + 2, 16, inner_h, text_background_image.texture);
    
}

WindowStyle::~WindowStyle(){
    for(unsigned i = 0; i < 8; i++)
        Lantern_DestroyPrimitive(p + i);
    delete[] p;
}

void WindowStyle::draw(unsigned x, unsigned y) const {
    
#define DRAW(WHICH, X, Y)\
    Lantern_DrawPrimitive(p + static_cast<int>(WHICH), (X) + x, (Y) + y)
    
    DRAW(eUL, 0, 0);
    DRAW(eUR, m_w - 16, 0);
    DRAW(eDR, m_w - 16, m_h - 16);
    DRAW(eDL, 0, m_h - 16);
    
    DRAW(eU, 16, 0);
    DRAW(eL, 0, 16);
    DRAW(eD, 16, m_h - 16);
    DRAW(eR, m_w - 16, 16);
    
    Lantern_Primitive *const bckgnd = p + static_cast<int>(eNumWSElements);
    Lantern_DrawPrimitive(bckgnd + 0, 16 + x, y + 16); 
    Lantern_DrawPrimitive(bckgnd + 1, 32 + x, y + 16); 
    Lantern_DrawPrimitive(bckgnd + 2, (m_w - 32) + x, y + 16); 
    
}

} // namespace Lantern