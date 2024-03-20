
UI_Comm
ui_text(Str8 key, u32 flags, Str8 text) {
  UI_Widget_Opts const opts = {
      .key    = key,
      .flags  = flags | UI_WidgetFlag_DrawText,
      .string = text,
      .semantic_size =
          {
              {.kind = UI_SizeKind_TextContent, .strictness = 0},
              {.kind = UI_SizeKind_TextContent, .strictness = 1},
          },
  };

  UI_Widget *result = ui_push_widget(opts);
  return {.widget = result};
}

template <typename... TArgs>
UI_Comm
ui_textf(Str8 key, u32 flags, char const* text, TArgs... args) {
  Str8 fmt = str8_sprintf(gContext.frame_arena, text, args...);
  return ui_text(key, flags, fmt);
}

UI_Comm
ui_rect(Str8 key, u32 flags, UI_Size size_x, UI_Size size_y) {
  UI_Widget_Opts const opts = {
      .key           = key,
      .flags         = flags,
      .semantic_size = {size_x, size_y},
  };

  UI_Widget *result = ui_push_widget(opts);
  return {.widget = result};
}
