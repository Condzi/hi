#pragma once

// For now assume that every font is monospace.
//
struct GFX_Font {
  GFX_Image image;
  u16       char_width;
  u16       char_height;
};

struct GFX_Glyph {
  GFX_Tex_Rect rect;      // Position inside the font's texture
  fvec2        sz;        // How big this glyph wants to be
  f32          advance_x; // How far you need to advance the pen to print next char.
};

must_use global GFX_Font
gfx_make_font(GFX_Image image, u16 char_width, u16 char_height);

must_use global GFX_Glyph
gfx_get_glyph(GFX_Font *font, u8 ch, u16 height_px);
