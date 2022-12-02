#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>
#include <windows.h>

/*
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
*/

#ifndef MEM_RESERVE
#define MEM_RESERVE 0x00002000
#endif

#ifndef MEM_COMMIT
#define MEM_COMMIT 0x00001000
#endif

struct HOOK_OBJECT {
	bool isEnabled;
	void* hook;
	void* original;
	void* trampoline;
	size_t len;
	std::vector<std::byte> stolenBytes;
};

class Trampo {
public:


	using byte_vector = std::vector<std::byte>;

	// Create a new trampoline hook
	std::unique_ptr<HOOK_OBJECT> CreateHook(void* hook, void* orig, size_t len);

	// Enable the trampoline hook
	bool EnableHook(HOOK_OBJECT* hookObj);

	// Disable the trampoline hook
	bool DisableHook(HOOK_OBJECT* hookObj);
};