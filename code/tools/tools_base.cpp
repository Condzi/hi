#pragma once
#include "all_inc.hpp"

void
tools_update() {
  if (!gDbgConsole.is_open) {
    return;
  }

  UI_Widget *bg = ui_push_widget({
      .key   = "debug_console_bg"_s8,
      .flags = UI_WidgetFlag_DrawBackground,
      .semantic_size =
          {
              {
                  .kind       = UI_SizeKind_PercentOfParent,
                  .value      = 1,
                  .strictness = 1,
              },
              {
                  .kind       = UI_SizeKind_PercentOfParent,
                  .value      = 0.35,
                  .strictness = 1,
              },
          },
  });

  ui_push_to_stack(bg);
  Defer { ui_pop_stack(); };

  u64 const         log_count = gContext.log->curr;
  Log_Header const *headers   = gContext.log->headers;
  Str8 const       *messages  = gContext.log->messages;

  for (u64 i = 0; i < log_count; i++) {
    Str8 const widget_key = str8_sprintf(gContext.frame_arena, "log_entry_%zu", i);
    Str8 const log_text   = str8_sprintf(gContext.frame_arena,
                                       "^w%.2f^| %S\n",
                                       log_timestamp_to_seconds(headers[i].timestamp),
                                       messages[i]);

    UI_Widget *log_widget = ui_text(widget_key, log_text, 0);
    Unused(log_widget);
  }
}
