#pragma once
#include "all_inc.hpp"

void
tools_update() {
  if (!gDbgConsole.is_open && !gDbgMemoryConsumption.is_open) {
    return;
  }

  UI_Widget *bg = ui_push_widget({
      .key   = "debug_console_bg"_s8,
      .flags = UI_WidgetFlag_DrawBackground | UI_WidgetFlag_AnimateVertical,
      .semantic_size =
          {
              {
                  .kind       = UI_SizeKind_PercentOfParent,
                  .value      = 1,
                  .strictness = 1,
              },
              {
                  .kind       = UI_SizeKind_ChildrenSum,
                  .strictness = 1,
              },
          },
  });

  ui_push_to_stack(bg);
  Defer { ui_pop_stack(); };

  {
    Str8 const console_header = str8_sprintf(gContext.frame_arena,
                                             "%.3fs | dt=todo | dt_min=todo | dt_max=todo",
                                             os_seconds_since_startup());
    UI_Widget *w              = ui_text("console_header"_s8, console_header, 0);
    Unused(w);
  }

  if (gDbgMemoryConsumption.is_open) {
    OS_Memory_Stats const stats         = os_get_memory_stats();
    Str8 const            memory_header = str8_sprintf(gContext.frame_arena,
                                            " Memory reserved: %_$$$zu | Commited: %_$$$zu",
                                            stats.reserved,
                                            stats.commited);

    UI_Widget *w = ui_text("memory_header"_s8, memory_header, UI_WidgetFlag_HorizontalLayout);
    Unused(w);
  }

  if (gDbgConsole.is_open) {
    u64 const         log_count = gContext.log->curr;
    Log_Header const *headers   = gContext.log->headers;
    Str8 const       *messages  = gContext.log->messages;
    u64 const         begin     = Max((int)log_count - (int)gDbgConsole.logs_max, 0);

    for (u64 i = begin; i < log_count; i++) {
      char severity = 'd';
      switch (headers[i].severity) {
        default:                  fallthrough;
        case LogSeverity_Debug:   severity = 'd'; break;
        case LogSeverity_Info:    severity = 'i'; break;
        case LogSeverity_Warning: severity = 'w'; break;
        case LogSeverity_Error:   severity = 'e'; break;
      }
      Str8 const widget_key = str8_sprintf(gContext.frame_arena, "log_entry_%zu", i);
      Str8 const log_text   = str8_sprintf(gContext.frame_arena,
                                         "^%c%.2f^| %S",
                                         severity,
                                         log_timestamp_to_seconds(headers[i].timestamp),
                                         messages[i]);

      UI_Widget *log_widget = ui_text(widget_key, log_text, 0);
      Unused(log_widget);
    }
  }
}
