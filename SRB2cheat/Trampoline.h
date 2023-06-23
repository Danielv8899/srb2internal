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

	//Activates the trampoline hook, takes pointer to hook object.
	int EnableHook(PHOOK_OBJECT HookObj);
	//Writes back the stolen bytes, takes pointer to hook object.
	BOOL DisableHook(PHOOK_OBJECT HookObj);
	//Creates an hook object. takes hook pointer, pointer to entry of bytes to steal and the length to steal.
	PHOOK_OBJECT CreateHook(LPVOID hook, LPVOID orig, size_t len);
}
