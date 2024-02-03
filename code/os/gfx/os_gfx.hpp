#pragma once

// Window Management
//

struct Window_Options {
  Str8 title;
  u32  width;
  u32  height;
  bool fullscreen;
};

global void
os_gfx_open_window(Window_Options opts);

global void
os_gfx_close_window();

enum Window_Mode {
  OS_WindowMode_Closed,
  OS_WindowMode_Open,
  OS_WindowMode_Minimized,
  OS_WindowMode_FullScreen
};

must_use global Window_Mode
os_gfx_window_mode();

global void
os_gfx_set_fullscreen(bool fullscreen);

must_use global u32
os_gfx_surface_width();

must_use global u32
os_gfx_surface_height();

must_use global f32
os_gfx_refresh_rate();

enum OS_Event_Type {
  OS_EventType_None = 0,

  // Window Events
  //
  OS_EventType_WindowClosed,
  OS_EventType_WindowLostFocus,
  OS_EventType_WindowGainedFocus,
  OS_EventType_WindowResized,

  // Button Events (Mouse + Keyboard)
  //
  OS_EventType_ButtonPressed,
  OS_EventType_ButtonReleased,

  // Scroll Events
  //
  OS_EventType_MouseScrolledV,
  OS_EventType_MouseScrolledH,

  OS_EventType__count
};

struct OS_Window_Event {
  OS_Window_Event *next;

  OS_Event_Type type;
  u64           timestamp_us;

  union {
    u32 button;
    f32 scroll;
  } data;
};

must_use global OS_Window_Event *
os_gfx_event_pump(Arena *arena);

no_return void
os_hard_fail(Str8 file, Str8 func, Str8 cnd, Str8 desc);
