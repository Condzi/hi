#pragma once
#include "all_inc.hpp"

internal Key_Bindings *gKeyBindings;

must_use Key_Bindings *
make_key_bindings(Arena *arena) {
  Key_Bindings *kb = arena_alloc<Key_Bindings>(arena);

  KB_Bind *b = kb->binds;

  b[KB_MoveUp].primary = b[KB_MoveUp].def = GameInput_LetterW;
  b[KB_MoveDown].primary = b[KB_MoveDown].def = GameInput_LetterS;
  b[KB_MoveLeft].primary = b[KB_MoveLeft].def = GameInput_LetterA;
  b[KB_MoveRight].primary = b[KB_MoveRight].def = GameInput_LetterD;
  b[KB_MenuBack].primary = b[KB_MenuBack].def = GameInput_Escape;
  b[KB_MenuEnter].primary = b[KB_MenuEnter].def = GameInput_Enter;

  return kb;
}

void
kb_set_key_bindings(Key_Bindings *kb) {
  LogEng_Info("[kb] Set new key bindings.");
  gKeyBindings = kb;
}

must_use Str8
kb_to_str8(KB_Type kb) {
  ErrorContext("kb=0x%X", (u32)kb);
  switch (kb) {
    default:           ErrorIf(true, "Key binding type is not recognised!");
    case KB_MoveUp:    return "Move Up"_s8;
    case KB_MoveDown:  return "Move Down"_s8;
    case KB_MoveLeft:  return "Move Left"_s8;
    case KB_MoveRight: return "Move Right"_s8;
    case KB_MenuBack:  return "Menu Back"_s8;
    case KB_MenuEnter: return "Menu Enter"_s8;
  }
}

must_use KB_Type_Array
kb_query_key_usage(Game_Input key) {
  ErrorContext("key=0x%X", (u32)key);
  ErrorIf(!gKeyBindings, "Key Bindings are not set.");

  KB_Type_Array res = {
      .v  = arena_alloc_array<u16>(gContext.frame_arena, gKeyBindings->sz),
      .sz = 0,
  };

  for (u64 i = 0; i < gKeyBindings->sz; i++) {
    if (gKeyBindings->binds[i].primary == key || gKeyBindings->binds[i].secondary == key) {
      res.v[res.sz] = (KB_Type)i;
      res.sz++;

      LogEng_Debug("[kb] Key already used: '%S' assigned to '%S'.",
                   game_input_to_str8(key),
                   kb_to_str8((KB_Type)i));
    }
  }

  Assert(res.sz < gKeyBindings->sz);
  return res;
}

void
kb_reset_one(KB_Type which) {
  ErrorContext("which=0x%X", (u32)which);
  ErrorIf(!gKeyBindings, "Key Bindings are not set.");

  u16 const idx            = (u16)which;
  u16       def_key        = gKeyBindings->binds[idx].def;
  gKeyBindings->binds[idx] = {
      .def       = def_key,
      .primary   = def_key,
      .secondary = 0,
  };

  LogEng_Info(
      "[kb] '%S' set to default '%S'.", kb_to_str8(which), game_input_to_str8((Game_Input)def_key));
}

void
kb_reset_all() {
  ErrorContext("");
  ErrorIf(!gKeyBindings, "Key Bindings are not set.");

  for (u64 i = 0; i < gKeyBindings->sz; i++) {
    u16 def_key            = gKeyBindings->binds[i].def;
    gKeyBindings->binds[i] = {
        .def       = def_key,
        .primary   = def_key,
        .secondary = 0,
    };
  }
  LogEng_Info("[kb] All set to default.");
}

must_use bool
kb_set_primary(KB_Type kb, Game_Input key) {
  ErrorContext("kb=%S, key=%S", kb_to_str8(kb), game_input_to_str8(key));
  ErrorIf(!gKeyBindings, "Key Bindings are not set.");

  if (kb_query_key_usage(key).sz) {
    return false;
  }
  gKeyBindings->binds[(u16)kb].primary = key;
  LogEng_Info("[kb] New primary for '%S': '%S'.", kb_to_str8(kb), game_input_to_str8(key));
  return true;
}

must_use bool
kb_set_secondary(KB_Type kb, Game_Input key) {
  ErrorContext("kb=%S, key=%S", kb_to_str8(kb), game_input_to_str8(key));
  ErrorIf(!gKeyBindings, "Key Bindings are not set.");

  if (kb_query_key_usage(key).sz) {
    return false;
  }
  gKeyBindings->binds[(u16)kb].secondary = key;
  LogEng_Info("[kb] New secondsary for '%S': '%S'.", kb_to_str8(kb), game_input_to_str8(key));
  return true;
}

must_use KB_State
kb_state(KB_Type which) {
  ErrorContext("which=0x%X", (u32)which);
  ErrorIf(!gKeyBindings, "Key Bindings are not set.");
  return gKeyBindings->states[(u16)which];
}

void
kb_update(OS_Window_Event *events) {
  ErrorContext("");
  ErrorIf(!gKeyBindings, "Key Bindings are not set.");

  // All keys that were 'released' in previous frame, now are not.
  //
  for (u64 i = 0; i < gKeyBindings->sz; i++) {
    gKeyBindings->states[i].released = false;
  }

  OS_Window_Event *it = events;
  while (it) {
    bool const is_released = (it->type == OS_EventType_ButtonReleased);
    bool const is_pressed  = (it->type != OS_EventType_ButtonPressed);
    if (!is_released && !is_pressed) {
      // We don't care.
      continue;
    }

    Game_Input const button = (Game_Input)it->data.button;
    for (u64 i = 0; i < gKeyBindings->sz; i++) {
      Game_Input const primary   = (Game_Input)gKeyBindings->binds[i].primary;
      Game_Input const secondary = (Game_Input)gKeyBindings->binds[i].secondary;
      if (button != primary && button != secondary) {
        continue;
      }
      KB_State &state = gKeyBindings->states[i];
      if (is_pressed) {
        if (state.pressed) {
          state = {.held = true};
        } else {
          state = {.pressed = true};
        }
      } else {
        state = {.released = true};
      }
    }

    it = it->next;
  }
}
