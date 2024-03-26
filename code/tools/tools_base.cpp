
void
tools_update() {
  if (!gDbgConsole.is_open && !gDbgMemoryConsumption.is_open) {
    return;
  }

  UI_Comm bg = ui_rect("debug_console_bg"_s8,
                       UI_WidgetFlag_DrawBackground | UI_WidgetFlag_AnimateVertical,
                       {.kind = UI_SizeKind_PercentOfParent, .value = 1, .strictness = 1},
                       {.kind = UI_SizeKind_ChildrenSum, .strictness = 1});

  ui_push_to_stack(bg.widget);
  Defer { ui_pop_stack(); };

  ui_textf("console_header"_s8,
           0,
           "%.3fs | dt=todo | dt_min=todo | dt_max=todo",
           os_seconds_since_startup());

  if (gDbgMemoryConsumption.is_open) {
    OS_Memory_Stats const stats = os_get_memory_stats();

    ui_textf("memory_header"_s8,
             UI_WidgetFlag_HorizontalLayout,
             " Memory reserved: %_$$$.3zu | Commited: %_$$$.3zu",
             stats.reserved,
             stats.commited);
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
      f32 const  timestamp  = log_timestamp_to_seconds(headers[i].timestamp);
      ui_textf(widget_key, 0, "^%c%.2f^| %S", severity, timestamp, messages[i]);
    }
  }
}

void
tools_init() {
  gDbgConsole.is_open  = true;
  gDbgConsole.logs_max = 24;
}
