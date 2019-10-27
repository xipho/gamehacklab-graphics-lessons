// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "detours/include/detours.h"

#pragma comment(lib,"detours/lib.X86/detours.lib")

// тайпдефы для хука
typedef BOOL(WINAPI* pEndPaint)(HWND hWnd, PAINTSTRUCT* ps);

// глобальные переменные
pEndPaint endPaintAddress;

// прототипы хук-функций
BOOL WINAPI HookedEndPaint(HWND hWnd, PAINTSTRUCT* ps);

LONG Hook(PVOID* hookAddr, PVOID hookFunc, bool hook = true);


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	HMODULE user32 = NULL;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		user32 = GetModuleHandle(L"user32.dll");
		if (user32) {
			endPaintAddress = (pEndPaint)GetProcAddress(user32, "EndPaint");
			Hook((PVOID *)&endPaintAddress, HookedEndPaint);
		}
		else {
			MessageBox(NULL, L"Something went wrong!", L"ERROR", MB_OK | MB_ICONERROR);
		}
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		if (user32) {
			Hook((PVOID *)&endPaintAddress, HookedEndPaint, false);
		}
        break;
    }
    return TRUE;
}

LONG Hook(PVOID* hookAddr, PVOID hookFunc, bool hook) {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	LONG result = 0L;
	result = hook ? DetourAttach(hookAddr, hookFunc) : DetourDetach(hookAddr, hookFunc);
	DetourTransactionCommit();
	return result;
}

BOOL WINAPI HookedEndPaint(HWND hWnd, PAINTSTRUCT* ps) {
	HBRUSH sampleBrush = CreateSolidBrush(RGB(255, 0, 0));
	HGDIOBJ oldBrush = SelectObject(ps->hdc, sampleBrush);
	Rectangle(ps->hdc, 100, 100, 500, 400);
	DeleteObject(SelectObject(ps->hdc, oldBrush));
	BOOL result = endPaintAddress(hWnd, ps);
	return result;
}