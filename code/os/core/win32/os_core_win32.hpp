#pragma once

// Minimal Win32 inclusion
//
#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS    // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NORASTEROPS      // Binary and Tertiary raster ops
#define OEMRESOURCE      // OEM Resource values
#define NODRAWTEXT       // DrawText() and DT_*
#define NOKERNEL         // All KERNEL defines and routines
#define NOMEMMGR         // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE       // typedef METAFILEPICT
#define NOMINMAX         // Macros min(a,b) and max(a,b)
#define NOOPENFILE       // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSERVICE        // All Service Controller routines, SERVICE_ equates, etc.
#define NOTEXTMETRIC     // typedef TEXTMETRIC and associated routines
#define NOWH             // SetWindowsHook and WH_*
#define NOCOMM           // COMM driver routines
#define NOKANJI          // Kanji support stuff.
#define NOHELP           // Help engine interface.
#define NOPROFILER       // Profiler interface.
#define NODEFERWINDOWPOS // DeferWindowPos routines
#define NOMCX            // Modem Configuration Extensions
#define NOMINMAX

#include <Windows.h>
#include <commctrl.h> // Task dialog

#include <shellapi.h>

// __debugbreak, OutputDebugString
//
#include <debugapi.h>

// MiniDump
//
#include <Dbghelp.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "shell32")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "Dbghelp")

#pragma comment(lib, "Comctl32")
// this is required for loading correct comctl32 dll file
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

