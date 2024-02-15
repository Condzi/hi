#pragma once

// OS-agnostic keys (+mouse buttons)
//

enum Game_Input {
  GameInput_Unknown,

  // Mouse buttons
  //
  GameInput_MouseLeft,
  GameInput_MouseRight,
  GameInput_MouseMiddle,
  GameInput_MouseExtra1,
  GameInput_MouseExtra2,

  GameInput_Backspace,
  GameInput_Tab,
  GameInput_Enter,
  GameInput_Shift,
  GameInput_Control,
  GameInput_Alt,
  GameInput_CapsLock,
  GameInput_Escape,
  GameInput_Space,
  GameInput_PageUp,
  GameInput_PageDown,
  GameInput_End,
  GameInput_Home,

  GameInput_LeftArrow,
  GameInput_UpArrow,
  GameInput_RightArrow,
  GameInput_DownArrow,

  GameInput_Select,
  GameInput_PrintScreen,
  GameInput_Insert,
  GameInput_Delete,

  GameInput_0,
  GameInput_1,
  GameInput_2,
  GameInput_3,
  GameInput_4,
  GameInput_5,
  GameInput_6,
  GameInput_7,
  GameInput_8,
  GameInput_9,

  GameInput_LetterA,
  GameInput_LetterB,
  GameInput_LetterC,
  GameInput_LetterD,
  GameInput_LetterE,
  GameInput_LetterF,
  GameInput_LetterG,
  GameInput_LetterH,
  GameInput_LetterI,
  GameInput_LetterJ,
  GameInput_LetterK,
  GameInput_LetterL,
  GameInput_LetterM,
  GameInput_LetterN,
  GameInput_LetterO,
  GameInput_LetterP,
  GameInput_LetterQ,
  GameInput_LetterR,
  GameInput_LetterS,
  GameInput_LetterT,
  GameInput_LetterU,
  GameInput_LetterV,
  GameInput_LetterW,
  GameInput_LetterX,
  GameInput_LetterY,
  GameInput_LetterZ,

  GameInput_LeftWin,
  GameInput_RightWin,

  GameInput_Numpad0,
  GameInput_Numpad1,
  GameInput_Numpad2,
  GameInput_Numpad3,
  GameInput_Numpad4,
  GameInput_Numpad5,
  GameInput_Numpad6,
  GameInput_Numpad7,
  GameInput_Numpad8,
  GameInput_Numpad9,
  GameInput_NumpadMultiply,
  GameInput_NumpadAdd,
  GameInput_NumpadSeparator,
  GameInput_NumpadSubtract,
  GameInput_NumpadDecimal,
  GameInput_NumpadDivide,
  GameInput_NumpadEquals,

  GameInput_F1,
  GameInput_F2,
  GameInput_F3,
  GameInput_F4,
  GameInput_F5,
  GameInput_F6,
  GameInput_F7,
  GameInput_F8,
  GameInput_F9,
  GameInput_F10,
  GameInput_F11,
  GameInput_F12,
  GameInput_F13,
  GameInput_F14,
  GameInput_F15,
  GameInput_F16,
  GameInput_F17,
  GameInput_F18,
  GameInput_F19,
  GameInput_F20,
  GameInput_F21,
  GameInput_F22,
  GameInput_F23,
  GameInput_F24,

  GameInput_NumLock,
  GameInput_ScrollLock,

  // @Robustness: Does this distinction even work?
  //
  GameInput_LeftShift,
  GameInput_RightShift,
  GameInput_LeftCtrl,
  GameInput_RightCtrl,
  GameInput_LeftMenu,
  GameInput_RightMenu,

  GameInput_Plus,
  GameInput_Comma,
  GameInput_Minus,
  GameInput_Period,

  GameInput_OEM1,   // ;:
  GameInput_OEM2,   // ?
  GameInput_OEM3,   // ~
  GameInput_OEM4,   // [
  GameInput_OEM5,   // backslash
  GameInput_OEM6,   // ]
  GameInput_OEM7,   // '
  GameInput_OEM102, // "\|"

  GameInput__count,
};

must_use global Str8
game_input_to_str8(Game_Input v);
