#pragma once

#include <EssaUtil/Rect.hpp>
#include <LLGL/OpenGL/Texture.hpp>
#include <map>

namespace llgl {

class TTFFont;

class GlyphCache {
public:
    explicit GlyphCache(int font_size);

    struct Glyph {
        Util::Rectu texture_rect;
    };

    Glyph ensure_glyph(TTFFont const& font, uint32_t codepoint);
    opengl::Texture const& atlas() const { return m_atlas; }

private:
    int m_font_size {};
    opengl::Texture m_atlas;
    std::map<uint32_t, Glyph> m_glyphs;
    Util::Vector2u m_current_atlas_position;
    int m_max_row_height = 0;
};

}