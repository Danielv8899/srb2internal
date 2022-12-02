#include "Trampoline.h"

std::unique_ptr<HOOK_OBJECT>Trampo::CreateHook(void* hook, void* orig, size_t len) {
	std::unique_ptr<HOOK_OBJECT> hookObj = std::make_unique<HOOK_OBJECT>();
	hookObj->len = len;
	hookObj->isEnabled = false;
	hookObj->hook = hook;
	hookObj->original = orig;

	// Steal the original bytes
	hookObj->stolenBytes = byte_vector(len);
	std::memcpy(hookObj->stolenBytes.data(), orig, len);

	// Allocate memory for the trampoline
	hookObj->trampoline = std::malloc(len + 5);
	if (!hookObj->trampoline) {
		throw std::bad_alloc();
	}

	// Copy the original bytes to the trampoline
	std::memcpy(hookObj->trampoline, hookObj->stolenBytes.data(), len);

	// Calculate the return address for the trampoline
	uintptr_t retAddr = reinterpret_cast<uintptr_t>(orig) + len - reinterpret_cast<uintptr_t>(hookObj->trampoline) + len + 5;

	// Write the return instruction to the trampoline
	std::memcpy(static_cast<std::byte*>(hookObj->trampoline) + len, "\xc3", 1);

	// Enable the hook
	return EnableHook(hookObj.get()) ? std::move(hookObj) : nullptr;
}

bool Trampo::EnableHook(HOOK_OBJECT* hookObj) {
	if (hookObj->isEnabled) {
		return false;
	}

	// Change the protection of the original function to allow modification
	std::uintptr_t oldProt;
	if (::VirtualProtect(hookObj->original, hookObj->len, PAGE_EXECUTE_READWRITE, reinterpret_cast<PDWORD>(&oldProt)) == 0) {
		throw std::runtime_error("failed VirtualProtect: " + std::to_string(::GetLastError()));
	}

	// Fill the original function with NOPs
	std::memset(hookObj->original, 0x90, hookObj->len);

	// Calculate the relative address of the hook function
	uintptr_t relAddr = reinterpret_cast<uintptr_t>(hookObj->hook) - reinterpret_cast<uintptr_t>(hookObj->original) - 5;

	// Write the jump instruction to the original function
	std::memcpy(hookObj->original, "\xe9", 1);
	std::memcpy(static_cast<std::byte*>(hookObj->original) + 1, &relAddr, sizeof(relAddr));

	// Restore the original protection of the function
	if (::VirtualProtect(hookObj->original, hookObj->len, oldProt, reinterpret_cast<PDWORD>(&oldProt)) == 0) {
		throw std::runtime_error("failed VirtualProtect: " + std::to_string(::GetLastError()));
	}

	hookObj->isEnabled = true;
	return true;
}

bool Trampo::DisableHook(HOOK_OBJECT* hookObj) {
	if (!hookObj->isEnabled) {
		return false;
	}

	// Change the protection of the original function to allow modification
	std::uintptr_t oldProt;
	if (::VirtualProtect(hookObj->original, hookObj->len, PAGE_EXECUTE_READWRITE, reinterpret_cast<PDWORD>(&oldProt)) == 0) {
		throw std::runtime_error("failed VirtualProtect: " + std::to_string(::GetLastError()));
	}

	// Restore the original bytes to the function
	std::memcpy(hookObj->original, hookObj->stolenBytes.data(), hookObj->len);

	// Restore the original protection of the function
	if (::VirtualProtect(hookObj->original, hookObj->len, oldProt, reinterpret_cast<PDWORD>(&oldProt)) == 0) {
		throw std::runtime_error("failed VirtualProtect: " + std::to_string(::GetLastError()));
	}

	hookObj->isEnabled = false;
	return true;
}
