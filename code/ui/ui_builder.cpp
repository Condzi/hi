#pragma once
#include "all_inc.hpp"

UI_Comm
ui_text(Str8 key, Str8 text, u32 flags) {
  UI_Widget_Opts const opts = {.key           = key,
                               .flags         = flags | UI_WidgetFlag_DrawText,
                               .string        = text,
                               .semantic_size = {
                                   {.kind = UI_SizeKind_TextContent, .strictness = 0},
                                   {.kind = UI_SizeKind_TextContent, .strictness = 1},
                               }};

  UI_Widget *result = ui_push_widget(opts);
  return {.widget = result};
}
