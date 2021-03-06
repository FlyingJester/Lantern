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
    Letter *m_letters;
    unsigned m_num_letters;

    struct Appender{
        const struct Sphere_Font *const m_font;
        const Letter *const m_letters;
        unsigned m_num_letters, m_at;
        unsigned m_x;
        const unsigned m_y;
        Appender(const struct Sphere_Font *font, Letter *letters,
            unsigned num_letters, unsigned x, unsigned y)
          : m_font(font)
          , m_letters(letters)
          , m_num_letters(num_letters)
          , m_at(0)
          , m_x(x)
          , m_y(y){
            
        }
        
        void operator() (char c){
            
            const Sphere_Glyph *const glyph = Sphere_GetBoundedGlyph(m_font, (unsigned)c);
            
            RECTANGLE_COORDS(vertex, m_x, m_y, glyph->w, glyph->h);
            RECTANGLE_COORDS(tex_coord,
                static_cast<float>(glyph->x) / static_cast<float>(m_font->master.w),
                0.0f,
                static_cast<float>(glyph->w) / static_cast<float>(m_font->master.w),
                static_cast<float>(glyph->h) / static_cast<float>(m_font->master.h)
            );
            
            m_x += glyph->w;
            
            LX_UploadBuffer(m_letters[m_at].vertex, vertex, sizeof(vertex));
            LX_UploadBuffer(m_letters[m_at].tex_coord, tex_coord, sizeof(tex_coord));

            m_at++;
        }
    };
public:
    
    TextLine(){ }
    
    void draw() const;
    void assign(const struct Sphere_Font *font, const char *text, unsigned x, unsigned y);
};

class Text {
    std::vector<TextLine> m_text;
    const struct Sphere_Font *const m_font;
    const LX_Texture m_texture;
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
    
    if(m_letters)
        delete[] m_letters;
    m_num_letters = i;
    m_letters = new Letter[i];
    
    Appender appender(font, m_letters, m_num_letters, x, y);
    
    std::for_each(text, text + i, appender);
}

void TextLine::draw() const {
    for(unsigned i = 0; i < m_num_letters; i++){
        Lantern_DrawPrimitiveLow(m_letters[i].vertex, m_letters[i].tex_coord, 4, eLX_Fan);
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
