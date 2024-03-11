#pragma once

#define GAME_MAJOR 0
#define GAME_MINOR 1
#define GAME_PATCH 0

#define GAME_VERSION_LITERAL                                                                       \
  Stringify(GAME_MAJOR) "." Stringify(GAME_MINOR) "." Stringify(GAME_PATCH) "." HI_GIT

#define GAME_TITLE_LITERAL "Home Invasion (ALPHA v" GAME_VERSION_LITERAL ") - " __DATE__
