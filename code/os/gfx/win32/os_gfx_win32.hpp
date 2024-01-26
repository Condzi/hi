#pragma once

// Internal objects
//

internal HINSTANCE        w32_h_instance;
internal HWND             w32_hwnd;
internal Window_Mode      w32_mode;
internal f32              w32_refresh_rate;
internal Arena           *w32_event_arena;
internal OS_Window_Event *w32_event_list;

// Where to restore the window after coming back from full screen
//
internal WINDOWPLACEMENT w32_window_placement;

internal LRESULT
win32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

internal OS_Window_Event *
win32_push_event(OS_Event_Type type);
