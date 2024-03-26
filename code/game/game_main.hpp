#pragma once

#define GAME_MAJOR 0
#define GAME_MINOR 1
#define GAME_PATCH 0

#define GAME_VERSION_LITERAL                                                                       \
  Stringify(GAME_MAJOR) "." Stringify(GAME_MINOR) "." Stringify(GAME_PATCH) "." HI_GIT

#define GAME_TITLE_LITERAL "Home Invasion (ALPHA v" GAME_VERSION_LITERAL ") - " __DATE__

struct Game_Master {
  PSX_World_ID psx_world;
  GFX_Camera   camera;

  // @States
  Sys_Logic *systems;

  // @Assets
  GFX_Font  font;
  GFX_Image bg;
  GFX_Image characters;

  f32 psx_accumulator;
} global gGameMaster;
