#pragma once
#include "all_inc.hpp"

must_use GFX_Font
gfx_make_font(GFX_Image image, u16 char_width, u16 char_height) {
  Assert(char_width > 1);
  Assert(char_height > 1);
  return {.image = image, .char_width = char_width, .char_height = char_height};
}

must_use GFX_Glyph
gfx_get_glyph(GFX_Font *font, u8 ch, u16 height_px) {
  ErrorContext("ch=%d, height_px=%g", (int)ch, height_px);
  ErrorIf(!font, "Font not set.");
  ErrorIf(height_px <= 4, "What is this, text for ants?");

  // Normalize the character ID. Calculate UV coords.
  //
  ch = Clamp(32, ch, 127);
  ch -= 32;

  u16 const u = (ch % 16) * font->char_width;
  u16 const v = (ch / 16) * font->char_height;

  // Calculate the desired width
  //
  f32 const w = ffloor(((f32)font->char_width * (f32)height_px) / (f32)font->char_height);

  return {
      .rect =
          {
              .x = u,
              .y = v,
              .w = font->char_width,
              .h = font->char_height,
          },
      .sz        = {w, (f32)height_px},
      .advance_x = w,
  };
}

must_use fvec2
gfx_size_rich_text(GFX_Rich_Text_Opts const &opts) {
  Assert(opts.string.sz);

  //@Note: This code must match gfX_draw_rich_text_color.
  u32 read_only local_persist TAB_SIZE = 4;
  // Ignore the starting position - we only want to size the text.
  fvec2     pen   = {};
  GFX_Glyph glyph = {};
  for (u64 i = 0; i < opts.string.sz; i++) {
    u8 const ch = opts.string.v[i];
    glyph       = gfx_get_glyph(opts.font, ch, opts.height_px);

    switch (ch) {
      default: {
        pen.x += glyph.advance_x;
      } break;

      case '\n': {
        pen.x = opts.pos.x;
        pen.y -= glyph.sz.height;
      } break;

      case '\t': {
        pen.x += glyph.advance_x * TAB_SIZE;
      } break;

      case '^': {
        if (i + 1 < opts.string.sz) {
          i++;
          u8 const modifier = opts.string.v[i];
          switch (modifier) {
            default: {
              i--; // the char does not set the color, so we render it.
            } break;

            case 'E':
            case 'e':
            case 'W':
            case 'w':
            case 'D':
            case 'd': {
            } break;
          }
        }
      } break;
    }
  }

  // Advance to get the bottom-right corner.
  pen.x += glyph.sz.x;
  pen.y -= glyph.sz.y;
  return pen;
}
