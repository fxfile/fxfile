#include "stdafx.h"
#include "fxfile-keyhook.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#pragma data_seg(".HOOKDATA")//Shared data among all instances.
HHOOK g_hHook = NULL;
HWND  g_hWnd = NULL;
UINT  g_uMsg = NULL;
WORD  g_wVirtualKeyCode = 0;
#pragma data_seg()

#pragma comment(linker, "/SECTION:.HOOKDATA,RWS")//linker directive

HINSTANCE g_hInstance = NULL;
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	g_hInstance = (HINSTANCE)hModule;

    return TRUE;
}

FXFILE_KEYHOOK_EXT_API BOOL InstallHook(HWND hWnd, UINT uMsg)
{
	g_hWnd = NULL;
	g_uMsg = 0;

	g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, g_hInstance, NULL);
	if (g_hHook)
	{
		g_hWnd = hWnd;
		g_uMsg = uMsg;
	}

	return g_hHook ? TRUE : FALSE;
}

FXFILE_KEYHOOK_EXT_API BOOL SetHookKey(WORD wVirtualKeyCode)
{
	if (!g_hHook)
		return FALSE;

	g_wVirtualKeyCode = wVirtualKeyCode;

	return TRUE;
}

FXFILE_KEYHOOK_EXT_API void RemoveHook()
{
	::UnhookWindowsHookEx(g_hHook);
	g_hHook = NULL;
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);

	KBDLLHOOKSTRUCT *pkbdhs = (KBDLLHOOKSTRUCT *)lParam;

	if (nCode == HC_ACTION)
	{
		if (g_wVirtualKeyCode != 0)
		{
			if (wParam == WM_KEYDOWN)
			{
				if (pkbdhs->vkCode == g_wVirtualKeyCode)
				{
					if ((GetAsyncKeyState(VK_LWIN) < 0) || (GetAsyncKeyState(VK_RWIN) < 0))
					{
						::PostMessage(g_hWnd, g_uMsg, wParam, lParam);
						return 1;
					}
				}
			}
		}
	}

	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}
