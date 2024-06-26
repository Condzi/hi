#pragma once

struct UI_Comm {
  UI_Widget *widget;
  bool       clicked;
};

UI_Comm
ui_text(Str8 key, u32 flags, Str8 text);

template <typename... TArgs>
UI_Comm
ui_textf(Str8 key, u32 flags, char const* text, TArgs... args);

UI_Comm
ui_rect(Str8 key, u32 flags, UI_Size size_x, UI_Size size_y);

template <typename... TArgs>
UI_Comm
ui_button(Str8 key, u32 flags, char const *text, TArgs... args);
