#pragma once
#include "all_inc.hpp"
#include <corecrt_wstdlib.h>
#include <cwchar>
#include <debugapi.h>
#include <winuser.h>

global void
os_gfx_init() {
  w32_h_instance = GetModuleHandleA(0);
  SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  // Create the window class.
  //
  WNDCLASSEXW const wnd_class = {
      .cbSize        = sizeof(wnd_class),
      .style         = CS_HREDRAW | CS_VREDRAW,
      .lpfnWndProc   = win32_window_proc,
      .hInstance     = w32_h_instance,
      .hCursor       = LoadCursorA(0, IDC_ARROW),
      .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
      .lpszClassName = L"game-window",
  };

  ATOM const wnd_class_atom = RegisterClassExW(&wnd_class);
  AssertAlways(wnd_class_atom);

  // Grab refresh rate
  //
  DEVMODEW devmodew;
  if (EnumDisplaySettingsW(0, ENUM_CURRENT_SETTINGS, &devmodew)) {
    w32_refresh_rate = (f32)devmodew.dmDisplayFrequency;
  }

  w32_window_placement.length = sizeof(w32_window_placement);
}

global void
os_gfx_open_window(Window_Options opts) {
  AssertAlways(opts.width > 0 && opts.width < 10'000);
  AssertAlways(opts.height > 0 && opts.height < 10'000);

  Str16      title = str16_from_8(gContext.frame_arena, opts.title);
  UINT const style = WS_OVERLAPPEDWINDOW;
  w32_hwnd         = CreateWindowExW(0,
                             L"game-window",
                             (WCHAR *)title.v,
                             style,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             (int)opts.width,
                             (int)opts.height,
                             0,
                             0,
                             w32_h_instance,
                             0);

  // U32 dpi = GetDpiForWindow(w32_hwnd);
  ShowWindow(w32_hwnd, SW_SHOW);
  UpdateWindow(w32_hwnd);

  w32_mode = OS_WindowMode_Open;

  if (opts.fullscreen) {
    os_gfx_set_fullscreen(true);
  }
}

global void
os_gfx_close_window() {
  PostMessageW(w32_hwnd, WM_CLOSE, 0, 0);
}

must_use global Window_Mode
os_gfx_window_mode() {
  return w32_mode;
}

global void
os_gfx_set_fullscreen(bool fullscreen) {
  DWORD window_style          = (DWORD)GetWindowLong(w32_hwnd, GWL_STYLE);
  bool  is_fullscreen_already = (w32_mode == OS_WindowMode_FullScreen);
  if (fullscreen) {
    if (!is_fullscreen_already) {
      GetWindowPlacement(w32_hwnd, &w32_window_placement);
    }
    MONITORINFO monitor_info = {.cbSize = sizeof(monitor_info)};
    if (GetMonitorInfo(MonitorFromWindow(w32_hwnd, MONITOR_DEFAULTTOPRIMARY),
                       &monitor_info)) {
      SetWindowLong(w32_hwnd, GWL_STYLE, (LONG)(window_style & ~WS_OVERLAPPEDWINDOW));
      SetWindowPos(w32_hwnd,
                   HWND_TOP,
                   monitor_info.rcMonitor.left,
                   monitor_info.rcMonitor.top,
                   monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                   monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                   SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
  } else {
    w32_mode = OS_WindowMode_Open;
    SetWindowLong(w32_hwnd, GWL_STYLE, (LONG)(window_style | WS_OVERLAPPEDWINDOW));
    SetWindowPlacement(w32_hwnd, &w32_window_placement);
    SetWindowPos(w32_hwnd,
                 0,
                 0,
                 0,
                 0,
                 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                     SWP_FRAMECHANGED);
  }
}

must_use global u32
os_gfx_surface_width() {
  ::RECT rect = {};
  ::GetClientRect(w32_hwnd, &rect);

  u32 h = (u32)(rect.right - rect.left);
  return h;
}

must_use global u32
os_gfx_surface_height() {
  ::RECT rect = {};
  ::GetClientRect(w32_hwnd, &rect);

  u32 h = (u32)(rect.bottom - rect.top);
  return h;
}

must_use global f32
os_gfx_refresh_rate() {
  return w32_refresh_rate;
}

must_use global OS_Window_Event *
os_gfx_event_pump(Arena *arena) {
  w32_event_arena = arena;
  w32_event_list  = 0;

  MSG msg;
  while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  if (msg.message == WM_QUIT) {
    win32_push_event(OS_EventType_WindowClosed);
    os_debug_message("WM_QUIT\n"_s8);
  }

  return w32_event_list;
}

internal LRESULT
win32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  // We're not ready for receiving events.
  //
  if (w32_event_arena == 0) {
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
  }

  LRESULT result = 0;
  switch (uMsg) {
    default: {
      result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
    } break;

    case WM_SYSCOMMAND: {
      switch (wParam) {
        case SC_MINIMIZE: {
          w32_mode = OS_WindowMode_Minimized;
        } break;
        case SC_MAXIMIZE: // fallthrough
        case SC_RESTORE: {
          w32_mode = OS_WindowMode_Open;
        } break;
      }
      os_debug_message("WM_SYSCOMMAND\n"_s8);
      result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
    } break;

    case WM_SIZE: {
      win32_push_event(OS_EventType_WindowResized);
      os_debug_message("WM_SIZE\n"_s8);
    } break;

    case WM_KILLFOCUS: {
      win32_push_event(OS_EventType_WindowLostFocus);
      os_debug_message("WM_KILLFOCUS\n"_s8);
    } break;

    case WM_SETFOCUS: {
      win32_push_event(OS_EventType_WindowGainedFocus);
      os_debug_message("WM_SETFOCUS\n"_s8);
    } break;

    case WM_KEYDOWN: {
      if (wParam == VK_ESCAPE) {
        DestroyWindow(hwnd);
        win32_push_event(OS_EventType_WindowClosed);
        w32_mode = OS_WindowMode_Closed;
      }
    } break;

    case WM_CLOSE: {
      win32_push_event(OS_EventType_WindowClosed);
      w32_mode = OS_WindowMode_Closed;
      DestroyWindow(w32_hwnd);
      os_debug_message("WM_CLOSE\n"_s8);
    } break;

    case WM_DESTROY: {
      os_debug_message("WM_DESTROY\n"_s8);
    } break;
  }
  return result;
}

internal OS_Window_Event *
win32_push_event(OS_Event_Type type) {
  OS_Window_Event *ev = arena_alloc<OS_Window_Event>(w32_event_arena);

  SLL_insert(w32_event_list, ev);
  ev->type         = type;
  ev->timestamp_us = os_now_us();

  return ev;
}

internal HRESULT WINAPI
win32_os_hard_fail_dialog_callback(
    HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LONG_PTR data) {
  Unused(data);
  Unused(wparam);
  Unused(hwnd);

  if (msg == TDN_HYPERLINK_CLICKED) {
    Str8 hyperlink_s8 =
        str8_from_16(gContext.frame_arena, str16_cstr((wchar_t const *)lparam));
    if (str8_has_prefix(hyperlink_s8, "mailto:"_s8)) {
      // It's the email address.
      //
      ShellExecuteW(0, L"open", (LPCWSTR)lparam, 0, 0, SW_SHOWNORMAL);
    } else {
      // It's a hyperlink to a source file, so open it in VS Code.
      //
      Str8 cmd8 =
          str8_sprintf(gContext.frame_arena, "code --goto -r %s"_s8, hyperlink_s8.v);
      Str16 cmd16 = str16_from_8(gContext.frame_arena, cmd8);
      _wsystem((wchar_t const *)(cmd16.v));
    }
  } else if (msg == TDN_BUTTON_CLICKED) {
    EndDialog(hwnd, 0);
  }
  return S_OK;
}

no_return void
os_hard_fail(Str8 file, Str8 func, Str8 cnd, Str8 desc) {
  Str8 const message_fmt = "Please, kindly send a screenshot of this message to <a "
                           "href=\"mailto:conradkubacki+bugreport@gmail.com\">"
                           "conradkubacki+bugreport@gmail.com</a>.\n"
                           "You can use ⊞ Win + ⇧ Shift + S to take a screenshot.\n\n"
                           "       %s\n"
                           "       %s\n"
                           "       (%s → <a href=\"%s\">%s</a>)\n"
                           "\n%s"_s8;

  Str8 err_ctx = "Error context is not available."_s8;
  if (gContext.error_context && gContext.error_context->first) {
    // Try to retrieve error context.
    //
    err_ctx                     = "\n"_s8;
    s32                 counter = 1;
    Error_Context_Node *node    = 0;
    for (node = gContext.error_context->first; node; node = node->next) {
      err_ctx = str8_sprintf(gContext.frame_arena,
                             "%s  %*s↪ %d. %s (%s → <a href=\"%s\">%s</a>)\n"_s8,
                             err_ctx.v,
                             counter * 2,
                             "",
                             counter,
                             node->desc.v,
                             node->function.v,
                             node->file.v,
                             node->file.v + ArrayCount("W:\\hi\\code"));
      counter++;
    }
  }

  Str8  final_message    = str8_sprintf(gContext.frame_arena,
                                    message_fmt,
                                    desc.v,
                                    cnd.v,
                                    func.v,
                                    file.v,
                                    file.v + ArrayCount("W:\\hi\\code"),
                                    err_ctx.v);
  Str16 final_message_16 = str16_from_8(gContext.frame_arena, final_message);

  TASKDIALOG_BUTTON debug_btn = {.nButtonID = 1001, .pszButtonText = L"Debug"};
  TASKDIALOGCONFIG  dialog    = {
          .cbSize = sizeof(dialog),
          .dwFlags =
          TDF_SIZE_TO_CONTENT | TDF_ENABLE_HYPERLINKS | TDF_ALLOW_DIALOG_CANCELLATION,
          .dwCommonButtons    = TDCBF_CLOSE_BUTTON,
          .pszWindowTitle     = L"Fatal Error :(",
          .pszMainIcon        = TD_ERROR_ICON,
          .pszMainInstruction = L"Program encountered an error and terminated.",
          .pszContent         = (LPCWSTR)final_message_16.v,
          .pszFooterIcon      = TD_INFORMATION_ICON,
          .pszFooter          = L"" GAME_TITLE_LITERAL,
          .pfCallback         = &win32_os_hard_fail_dialog_callback,
  };

#if !defined(NDEBUG)
  dialog.pButtons = &debug_btn;
  dialog.cButtons = 1;
#endif

  int debug_btn_status = 0;
  TaskDialogIndirect(&dialog, &debug_btn_status, 0, 0);

  if (debug_btn_status == 1001) {
    Trap();
  }
  exit(0);
}
