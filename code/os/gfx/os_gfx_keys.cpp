#pragma once
#include "all_inc.hpp"

must_use global Str8
game_input_to_str8(Game_Input v) {
  switch (v) {
    default: {
      AssertAlways(false);
    } /* fall through */
    case GameInput_Unknown:
      return "Unknown Key"_s8;
    case GameInput_MouseLeft:
      return "Left Mouse Button"_s8;
    case GameInput_MouseRight:
      return "Right Mouse Button"_s8;
    case GameInput_MouseMiddle:
      return "Middle Mouse Button"_s8;
    case GameInput_MouseExtra1:
      return "1st Extra Mouse Button"_s8;
    case GameInput_MouseExtra2:
      return "2nd Extra Mouse Button"_s8;
    case GameInput_Backspace:
      return "Backspace"_s8;
    case GameInput_Tab:
      return "Tab"_s8;
    case GameInput_Enter:
      return "Enter"_s8;
    case GameInput_Shift:
      return "Shift"_s8;
    case GameInput_Control:
      return "Control"_s8;
    case GameInput_Alt:
      return "Alt"_s8;
    case GameInput_CapsLock:
      return "CapsLock"_s8;
    case GameInput_Escape:
      return "Escape"_s8;
    case GameInput_Space:
      return "Space"_s8;
    case GameInput_PageUp:
      return "PageUp"_s8;
    case GameInput_PageDown:
      return "PageDown"_s8;
    case GameInput_End:
      return "End"_s8;
    case GameInput_Home:
      return "Home"_s8;
    case GameInput_LeftArrow:
      return "Left Arrow"_s8;
    case GameInput_UpArrow:
      return "Up Arrow"_s8;
    case GameInput_RightArrow:
      return "Right Arrow"_s8;
    case GameInput_DownArrow:
      return "Down Arrow"_s8;
    case GameInput_Select:
      return "Select"_s8;
    case GameInput_PrintScreen:
      return "PrintScreen"_s8;
    case GameInput_Insert:
      return "Insert"_s8;
    case GameInput_Delete:
      return "Delete"_s8;
    case GameInput_0:
      return "0"_s8;
    case GameInput_1:
      return "1"_s8;
    case GameInput_2:
      return "2"_s8;
    case GameInput_3:
      return "3"_s8;
    case GameInput_4:
      return "4"_s8;
    case GameInput_5:
      return "5"_s8;
    case GameInput_6:
      return "6"_s8;
    case GameInput_7:
      return "7"_s8;
    case GameInput_8:
      return "8"_s8;
    case GameInput_9:
      return "9"_s8;
    case GameInput_LetterA:
      return "A"_s8;
    case GameInput_LetterB:
      return "B"_s8;
    case GameInput_LetterC:
      return "C"_s8;
    case GameInput_LetterD:
      return "D"_s8;
    case GameInput_LetterE:
      return "E"_s8;
    case GameInput_LetterF:
      return "F"_s8;
    case GameInput_LetterG:
      return "G"_s8;
    case GameInput_LetterH:
      return "H"_s8;
    case GameInput_LetterI:
      return "I"_s8;
    case GameInput_LetterJ:
      return "J"_s8;
    case GameInput_LetterK:
      return "K"_s8;
    case GameInput_LetterL:
      return "L"_s8;
    case GameInput_LetterM:
      return "M"_s8;
    case GameInput_LetterN:
      return "N"_s8;
    case GameInput_LetterO:
      return "O"_s8;
    case GameInput_LetterP:
      return "P"_s8;
    case GameInput_LetterQ:
      return "Q"_s8;
    case GameInput_LetterR:
      return "R"_s8;
    case GameInput_LetterS:
      return "S"_s8;
    case GameInput_LetterT:
      return "T"_s8;
    case GameInput_LetterU:
      return "U"_s8;
    case GameInput_LetterV:
      return "V"_s8;
    case GameInput_LetterW:
      return "W"_s8;
    case GameInput_LetterX:
      return "X"_s8;
    case GameInput_LetterY:
      return "Y"_s8;
    case GameInput_LetterZ:
      return "Z"_s8;
    case GameInput_LeftWin:
      return "Left Win"_s8;
    case GameInput_RightWin:
      return "Right Win"_s8;
    case GameInput_Numpad0:
      return "Numpad 0"_s8;
    case GameInput_Numpad1:
      return "Numpad 1"_s8;
    case GameInput_Numpad2:
      return "Numpad 2"_s8;
    case GameInput_Numpad3:
      return "Numpad 3"_s8;
    case GameInput_Numpad4:
      return "Numpad 4"_s8;
    case GameInput_Numpad5:
      return "Numpad 5"_s8;
    case GameInput_Numpad6:
      return "Numpad 6"_s8;
    case GameInput_Numpad7:
      return "Numpad 7"_s8;
    case GameInput_Numpad8:
      return "Numpad 8"_s8;
    case GameInput_Numpad9:
      return "Numpad 9"_s8;
    case GameInput_NumpadMultiply:
      return "Numpad Multiply"_s8;
    case GameInput_NumpadAdd:
      return "Numpad Add"_s8;
    case GameInput_NumpadSeparator:
      return "Numpad Separator"_s8;
    case GameInput_NumpadSubtract:
      return "Numpad Subtract"_s8;
    case GameInput_NumpadDecimal:
      return "Numpad Decimal"_s8;
    case GameInput_NumpadDivide:
      return "Numpad Divide"_s8;
    case GameInput_NumpadEquals:
      return "Numpad Equals"_s8;
    case GameInput_F1:
      return "F1"_s8;
    case GameInput_F2:
      return "F2"_s8;
    case GameInput_F3:
      return "F3"_s8;
    case GameInput_F4:
      return "F4"_s8;
    case GameInput_F5:
      return "F5"_s8;
    case GameInput_F6:
      return "F6"_s8;
    case GameInput_F7:
      return "F7"_s8;
    case GameInput_F8:
      return "F8"_s8;
    case GameInput_F9:
      return "F9"_s8;
    case GameInput_F10:
      return "F10"_s8;
    case GameInput_F11:
      return "F11"_s8;
    case GameInput_F12:
      return "F12"_s8;
    case GameInput_F13:
      return "F13"_s8;
    case GameInput_F14:
      return "F14"_s8;
    case GameInput_F15:
      return "F15"_s8;
    case GameInput_F16:
      return "F16"_s8;
    case GameInput_F17:
      return "F17"_s8;
    case GameInput_F18:
      return "F18"_s8;
    case GameInput_F19:
      return "F19"_s8;
    case GameInput_F20:
      return "F20"_s8;
    case GameInput_F21:
      return "F21"_s8;
    case GameInput_F22:
      return "F22"_s8;
    case GameInput_F23:
      return "F23"_s8;
    case GameInput_F24:
      return "F24"_s8;
    case GameInput_NumLock:
      return "NumLock"_s8;
    case GameInput_ScrollLock:
      return "ScrollLock"_s8;
    case GameInput_LeftShift:
      return "Left Shift"_s8;
    case GameInput_RightShift:
      return "Right Shift"_s8;
    case GameInput_LeftCtrl:
      return "Lef Ctrl"_s8;
    case GameInput_RightCtrl:
      return "Right Ctrl"_s8;
    case GameInput_LeftMenu:
      return "Left Menu"_s8;
    case GameInput_RightMenu:
      return "Right Menu"_s8;
    case GameInput_Plus:
      return "Plus"_s8;
    case GameInput_Comma:
      return "Comma"_s8;
    case GameInput_Minus:
      return "Minus"_s8;
    case GameInput_Period:
      return "Period"_s8;
    case GameInput_OEM1:
      return ";:"_s8;
    case GameInput_OEM2:
      return "/"_s8;
    case GameInput_OEM3:
      return "~"_s8;
    case GameInput_OEM4:
      return "["_s8;
    case GameInput_OEM5:
      return "\\"_s8;
    case GameInput_OEM6:
      return "]"_s8;
    case GameInput_OEM7:
      return "'"_s8;
    case GameInput_OEM102:
      return "\\|"_s8;
  }
}
