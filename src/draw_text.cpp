#include "draw_text.h"

#include "primitives.h"
#include "primitives_low.h"

#include <vector>
#include <algorithm>
#include <cstring>

namespace Lantern {

struct Letter {
    const LX_Buffer vertex, tex_coord;

    Letter(LX_Buffer v, LX_Buffer t) : vertex(v), tex_coord(t) { }
    Letter() : vertex(LX_CreateBuffer()), tex_coord(LX_CreateBuffer()) { }

    ~Letter() {
        LX_DestroyBuffer(vertex);
        LX_DestroyBuffer(tex_coord);
    }
};

class TextLine {
    std::vector<Letter> m_letters;
    
    struct Appender{
        const struct Sphere_Font *const m_font;
        std::vector<Letter> &m_letters;
        unsigned m_x;
        const unsigned m_y;
        Appender(const struct Sphere_Font *font,
            std::vector<Letter> &letters, unsigned x, unsigned y)
          : m_font(font)
          , m_letters(letters)
          , m_x(x)
          , m_y(y){
            
        }
        
        void operator() (char c){
            
            m_letters.resize(m_letters.size() + 1);

            const Sphere_Glyph *const glyph = Sphere_GetBoundedGlyph(m_font, (unsigned)c);
            
            RECTANGLE_COORDS(vertex, m_x, m_y, glyph->w, glyph->h);
            RECTANGLE_COORDS(tex_coord,
                static_cast<float>(glyph->x) / static_cast<float>(m_font->master.w),
                0.0f,
                static_cast<float>(glyph->w) / static_cast<float>(m_font->master.w),
                static_cast<float>(glyph->h) / static_cast<float>(m_font->master.h)
            );
            
            m_x += glyph->w;
            
            LX_UploadBuffer(m_letters.back().vertex, vertex, sizeof(vertex));
            LX_UploadBuffer(m_letters.back().tex_coord, tex_coord, sizeof(tex_coord));
        }
    };
public:
    
    TextLine(){ }
    
    void draw() const;
    void assign(const struct Sphere_Font *font, const char *text, unsigned x, unsigned y);
};

class Text {
    std::vector<TextLine> m_text;
    const LX_Texture m_texture;
    const struct Sphere_Font *const m_font;
public:
    Text(const struct Sphere_Font *font)
      : m_font(font)
      , m_texture(LX_CreateTexture()){
        LX_UploadTexture(m_texture, font->master.data, font->master.w, font->master.h);
    }
    
    ~Text() {
        LX_DestroyTexture(m_texture);
    }

    void draw() const;
    
    inline void append(const char *text, unsigned x, unsigned y){
        m_text.resize(m_text.size() + 1);
        m_text.back().assign(m_font, text, x, y);
    }
    
    inline void clear() { m_text.clear(); }
};

void TextLine::assign(const struct Sphere_Font *font, const char *text, unsigned x, unsigned y){
    const unsigned i = strlen(text);
    
    m_letters.clear();
    m_letters.reserve(i);
    
    Appender appender(font, m_letters, x, y);
    
    std::for_each(text, text + i, appender);
}

void TextLine::draw() const {
    for(std::vector<Letter>::const_iterator i = m_letters.begin();
        i != m_letters.end(); i++){
        Lantern_DrawPrimitiveLow(i->vertex, i->tex_coord, 4, eLX_Fan);
    }
}

void Text::draw() const {
    LX_SetTexture(m_texture);
    for(std::vector<TextLine>::const_iterator i = m_text.begin();
        i != m_text.end(); i++){
        i->draw();
    }
}

} // namespace Lantern

using Lantern::Text;

struct Lantern_FontContext *Lantern_CreateFontContext(){
    return (Lantern_FontContext*)(new Text(Sphere_GetSystemFont()));
}

void Lantern_DestroyFontContext(struct Lantern_FontContext *ctx){
    delete (Text *)ctx;
}

void Lantern_AddTextToFontContext(
    struct Lantern_FontContext *LANTERN_RESTRICT ctx,
    const char *LANTERN_RESTRICT text, 
    unsigned x, unsigned y){
    ((Text *)ctx)->append(text, x, y);
}

void Lantern_DrawFontContext(const struct Lantern_FontContext *ctx){
    ((Text *)ctx)->draw();
}

void Lantern_ClearFontContext(struct Lantern_FontContext *ctx){
    ((Text *)ctx)->clear();
}
