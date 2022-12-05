#pragma once
#include <windows.h>
#include <iostream>

typedef struct _HOOK_OBJECT {
	BOOL isEnabled;
	LPVOID hook;
	LPVOID original;
	LPVOID trampoline;
	PBYTE stolenBytes;
	size_t len;
} HOOK_OBJECT, * PHOOK_OBJECT;

namespace Trampo {

	BOOL EnableHook(PHOOK_OBJECT HookObj);
	BOOL DisableHook(PHOOK_OBJECT HookObj);
	PHOOK_OBJECT CreateHook(LPVOID hook, LPVOID orig, size_t len);
}
