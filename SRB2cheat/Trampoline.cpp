#include "Trampoline.h"

BOOL Trampo::EnableHook(PHOOK_OBJECT HookObj) {

	if (HookObj->isEnabled) {
		printf("hook is already enabled\n");
		return FALSE;
	}

	BYTE stolenBytes[5] = { 0 };
	DWORD oldProt;

	LPVOID orig = HookObj->original;
	LPVOID hook = HookObj->hook;

	if (!VirtualProtect(orig, 5, PAGE_EXECUTE_READWRITE, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}

	DWORD relAddr = (DWORD)hook - (DWORD)orig - 5;

	*(BYTE*)orig = 0xE9;
	orig = (LPVOID)((DWORD)orig + 1);
	*(DWORD*)orig = (DWORD)relAddr;

	if (!VirtualProtect(orig, 5, oldProt, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}

	HookObj->isEnabled = true;
	return TRUE;
}

BOOL Trampo::DisableHook(PHOOK_OBJECT HookObj)
{
	if (!HookObj->isEnabled) {
		printf("hook is not enabled\n");
		return FALSE;
	}

	DWORD oldProt;

	if (!VirtualProtect(HookObj->original, 5, PAGE_EXECUTE_READWRITE, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}

	if (!RtlCopyMemory(HookObj->original, HookObj->stolenBytes, 5)) {
		printf("failed writing bytes: %d\n", GetLastError());
		exit(-1);
	}

	if (!VirtualProtect(HookObj->original, 5, oldProt, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}

	HookObj->isEnabled = false;
	return TRUE;
}

PHOOK_OBJECT Trampo::CreateHook(LPVOID hook, LPVOID orig)
{
	PHOOK_OBJECT hookObj = (PHOOK_OBJECT)malloc(sizeof(HOOK_OBJECT));
	if (!hookObj) {
		printf("failed mallocing hook object\n");
		exit(-1);
	}

	RtlZeroMemory(hookObj, sizeof(HOOK_OBJECT));

	hookObj->isEnabled = false;
	hookObj->hook = hook;
	hookObj->original = orig;

	BYTE* stolenBytes = (BYTE*)malloc(5);
	if (!stolenBytes) {
		printf("failed mallocing stolen bytes\n");
		exit(-1);
	}

	if (!RtlCopyMemory(stolenBytes, orig, 5)) {
		printf("failed stealing bytes: %d\n", GetLastError());
		exit(-1);
	}

	hookObj->stolenBytes = stolenBytes;

	LPVOID tramp = VirtualAlloc(0, 10, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!tramp) {
		printf("failed VirtualAlloc: %d\n", GetLastError());
		exit(-1);
	}

	hookObj->trampoline = tramp;

	if (!RtlCopyMemory(tramp, stolenBytes, 5)) {
		printf("failed writing bytes: %d\n", GetLastError());
		exit(-1);
	}

	DWORD retAddr = ((DWORD)tramp + 5) - ((DWORD)orig + 5) - 5;

	tramp = (LPVOID)((DWORD)tramp + 5);
	*(BYTE*)tramp = 0xE9;
	tramp = (LPVOID)((DWORD)tramp + 1);
	*(DWORD*)tramp = retAddr;

	return hookObj;
}

