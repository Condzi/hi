#pragma once

struct UI_Comm {
  UI_Widget *widget;
  bool       clicked;
};

UI_Comm
ui_text(Str8 key, Str8 text, u32 flags);
