#pragma once
#include <windows.h>
#include <iostream>

namespace Trampo {

	void EnableHook(LPVOID hook, LPVOID orig, LPVOID* trampoline, PBYTE* stolen);
	void DisableHook(LPVOID orig, LPVOID trampoline, PBYTE stolenBytes);
}