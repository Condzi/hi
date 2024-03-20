
must_use Game_Input
os_gfx_vk_to_game_input(u32 vk) {
  switch (vk) {
    default:               return GameInput_Unknown;
    case VK_LBUTTON:       return GameInput_MouseLeft;
    case VK_RBUTTON:       return GameInput_MouseRight;
    case VK_MBUTTON:       return GameInput_MouseMiddle;
    case VK_XBUTTON1:      return GameInput_MouseExtra1;
    case VK_XBUTTON2:      return GameInput_MouseExtra2;
    case VK_BACK:          return GameInput_Backspace;
    case VK_TAB:           return GameInput_Tab;
    case VK_RETURN:        return GameInput_Enter;
    case VK_SHIFT:         return GameInput_Shift;
    case VK_CONTROL:       return GameInput_Control;
    case VK_MENU:          return GameInput_Alt;
    case VK_CAPITAL:       return GameInput_CapsLock;
    case VK_ESCAPE:        return GameInput_Escape;
    case VK_SPACE:         return GameInput_Space;
    case VK_PRIOR:         return GameInput_PageUp;
    case VK_NEXT:          return GameInput_PageDown;
    case VK_END:           return GameInput_End;
    case VK_HOME:          return GameInput_Home;
    case VK_LEFT:          return GameInput_LeftArrow;
    case VK_UP:            return GameInput_UpArrow;
    case VK_RIGHT:         return GameInput_RightArrow;
    case VK_DOWN:          return GameInput_DownArrow;
    case VK_SELECT:        return GameInput_Select;
    case VK_PRINT:         return GameInput_PrintScreen;
    case VK_INSERT:        return GameInput_Insert;
    case VK_DELETE:        return GameInput_Delete;
    case '0':              return GameInput_0;
    case '1':              return GameInput_1;
    case '2':              return GameInput_2;
    case '3':              return GameInput_3;
    case '4':              return GameInput_4;
    case '5':              return GameInput_5;
    case '6':              return GameInput_6;
    case '7':              return GameInput_7;
    case '8':              return GameInput_8;
    case '9':              return GameInput_9;
    case 'A':              return GameInput_LetterA;
    case 'B':              return GameInput_LetterB;
    case 'C':              return GameInput_LetterC;
    case 'D':              return GameInput_LetterD;
    case 'E':              return GameInput_LetterE;
    case 'F':              return GameInput_LetterF;
    case 'G':              return GameInput_LetterG;
    case 'H':              return GameInput_LetterH;
    case 'I':              return GameInput_LetterI;
    case 'J':              return GameInput_LetterJ;
    case 'K':              return GameInput_LetterK;
    case 'L':              return GameInput_LetterL;
    case 'M':              return GameInput_LetterM;
    case 'N':              return GameInput_LetterN;
    case 'O':              return GameInput_LetterO;
    case 'P':              return GameInput_LetterP;
    case 'Q':              return GameInput_LetterQ;
    case 'R':              return GameInput_LetterR;
    case 'S':              return GameInput_LetterS;
    case 'T':              return GameInput_LetterT;
    case 'U':              return GameInput_LetterU;
    case 'V':              return GameInput_LetterV;
    case 'W':              return GameInput_LetterW;
    case 'X':              return GameInput_LetterX;
    case 'Y':              return GameInput_LetterY;
    case 'Z':              return GameInput_LetterZ;
    case VK_LWIN:          return GameInput_LeftWin;
    case VK_RWIN:          return GameInput_RightWin;
    case VK_NUMPAD0:       return GameInput_Numpad0;
    case VK_NUMPAD1:       return GameInput_Numpad1;
    case VK_NUMPAD2:       return GameInput_Numpad2;
    case VK_NUMPAD3:       return GameInput_Numpad3;
    case VK_NUMPAD4:       return GameInput_Numpad4;
    case VK_NUMPAD5:       return GameInput_Numpad5;
    case VK_NUMPAD6:       return GameInput_Numpad6;
    case VK_NUMPAD7:       return GameInput_Numpad7;
    case VK_NUMPAD8:       return GameInput_Numpad8;
    case VK_NUMPAD9:       return GameInput_Numpad9;
    case VK_MULTIPLY:      return GameInput_NumpadMultiply;
    case VK_ADD:           return GameInput_NumpadAdd;
    case VK_SEPARATOR:     return GameInput_NumpadSeparator;
    case VK_SUBTRACT:      return GameInput_NumpadSubtract;
    case VK_DECIMAL:       return GameInput_NumpadDecimal;
    case VK_DIVIDE:        return GameInput_NumpadDivide;
    case VK_OEM_NEC_EQUAL: return GameInput_NumpadEquals;
    case VK_F1:            return GameInput_F1;
    case VK_F2:            return GameInput_F2;
    case VK_F3:            return GameInput_F3;
    case VK_F4:            return GameInput_F4;
    case VK_F5:            return GameInput_F5;
    case VK_F6:            return GameInput_F6;
    case VK_F7:            return GameInput_F7;
    case VK_F8:            return GameInput_F8;
    case VK_F9:            return GameInput_F9;
    case VK_F10:           return GameInput_F10;
    case VK_F11:           return GameInput_F11;
    case VK_F12:           return GameInput_F12;
    case VK_F13:           return GameInput_F13;
    case VK_F14:           return GameInput_F14;
    case VK_F15:           return GameInput_F15;
    case VK_F16:           return GameInput_F16;
    case VK_F17:           return GameInput_F17;
    case VK_F18:           return GameInput_F18;
    case VK_F19:           return GameInput_F19;
    case VK_F20:           return GameInput_F20;
    case VK_F21:           return GameInput_F21;
    case VK_F22:           return GameInput_F22;
    case VK_F23:           return GameInput_F23;
    case VK_F24:           return GameInput_F24;
    case VK_NUMLOCK:       return GameInput_NumLock;
    case VK_SCROLL:        return GameInput_ScrollLock;
    case VK_LSHIFT:        return GameInput_LeftShift;
    case VK_RSHIFT:        return GameInput_RightShift;
    case VK_LCONTROL:      return GameInput_LeftCtrl;
    case VK_RCONTROL:      return GameInput_RightCtrl;
    case VK_LMENU:         return GameInput_LeftAlt;
    case VK_RMENU:         return GameInput_RightAlt;
    case VK_OEM_PLUS:      return GameInput_Plus;
    case VK_OEM_COMMA:     return GameInput_Comma;
    case VK_OEM_MINUS:     return GameInput_Minus;
    case VK_OEM_PERIOD:    return GameInput_Period;
    case VK_OEM_1:         return GameInput_OEM1; // For ';' key
    case VK_OEM_2:         return GameInput_OEM2; // For '/' key
    case VK_OEM_3:         return GameInput_OEM3; // For '`' key
    case VK_OEM_4:         return GameInput_OEM4; // For '[' key
    case VK_OEM_5:         return GameInput_OEM5; // For '\\' key
    case VK_OEM_6:         return GameInput_OEM6; // For ']' key
    case VK_OEM_7:         return GameInput_OEM7; // For '\'' key
    case VK_OEM_102:       return GameInput_OEM102;     // For the '<' or '|' key
  }
}
