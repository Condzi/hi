#pragma once
#include "all_inc.hpp"

must_use global GFX_Font
gfx_make_font(GFX_Image image, u16 char_width, u16 char_height) {
  Assert(char_width > 1);
  Assert(char_height > 1);
  return {.image = image, .char_width = char_width, .char_height = char_height};
}

must_use global GFX_Glyph
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
