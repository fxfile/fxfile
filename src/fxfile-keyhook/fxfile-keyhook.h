// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FXKEYHOOK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FXKEYHOOK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FXFILE_KEYHOOK_EXPORTS
#define FXFILE_KEYHOOK_EXT_API __declspec(dllexport)
#else
#define FXFILE_KEYHOOK_EXT_API __declspec(dllimport)
#endif

FXFILE_KEYHOOK_EXT_API BOOL InstallHook(HWND hWnd, UINT uMsg);
FXFILE_KEYHOOK_EXT_API BOOL SetHookKey(WORD wVirtualKeyCode);
FXFILE_KEYHOOK_EXT_API void RemoveHook();
