#pragma once

enum KB_Type {
  KB_MoveUp,
  KB_MoveDown,
  KB_MoveLeft,
  KB_MoveRight,

  KB_MenuBack,
  KB_MenuEnter,

  KB__count
};

struct KB_Type_Array {
  u16 *v;
  u64  sz;
};

struct KB_Bind {
  u16 def;
  u16 primary;
  u16 secondary;
};

struct KB_State {
  u8 pressed  : 1;
  u8 released : 1;
  u8 held     : 1;
};

struct Key_Bindings {
  KB_State             states[KB__count];
  KB_Bind              binds[KB__count];
  u64 static read_only sz = KB__count;
};

must_use Key_Bindings *
make_key_bindings(Arena *arena);

void
kb_set_key_bindings(Key_Bindings *kb);

must_use Str8
kb_to_str8(KB_Type kb);

// Returns key bindings that use this key their secondary or primary.
//
must_use KB_Type_Array
kb_query_key_usage(Game_Input key);

void
kb_reset_one(KB_Type which);

void
kb_reset_all();

must_use bool
kb_set_primary(KB_Type kb, Game_Input key);

must_use bool
kb_set_secondary(KB_Type kb, Game_Input key);

must_use KB_State
kb_state(KB_Type which);

void
kb_update(OS_Window_Event* events);
