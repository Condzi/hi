#pragma once
#include "all_inc.hpp"

read_only internal u32 GFX_CHK_LIGHT = 0xFFC3C3C3;
read_only internal u32 GFX_CHK_DARK  = 0xFF7F7F7F;

must_use global u8 *
gfx_checkerboard(u32 square_size, u32 width_squares, u32 height_squares) {
  u32 const width_px  = width_squares * square_size;
  u32 const height_px = height_squares * square_size;
  u32      *tex       = arena_alloc_array<u32>(gContext.frame_arena, (u64)width_px * height_px);

  for (u32 y = 0; y < width_px; ++y) {
    for (u32 x = 0; x < height_px; ++x) {
      // Determine if we're on a "black" or "white" square based on the current pixel's position.
      bool is_dark          = ((x / square_size) % 2 == (y / square_size) % 2);
      tex[y * width_px + x] = is_dark ? GFX_CHK_DARK : GFX_CHK_LIGHT; // AARRGGBB format
    }
  }

  return (u8 *)tex;
}
