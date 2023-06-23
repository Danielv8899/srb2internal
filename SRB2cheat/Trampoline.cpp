#include "Trampoline.h"

int Trampo::EnableHook(PHOOK_OBJECT HookObj) {

	if (HookObj->isEnabled) {
		printf("hook is already enabled\n");
		return 0;
	}

	DWORD oldProt;

	LPVOID orig = HookObj->original;
	LPVOID hook = HookObj->hook;

	if (!VirtualProtect(orig, HookObj->len, PAGE_EXECUTE_READWRITE, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		return -1;
	}

	RtlFillMemory(orig, HookObj->len, 0x90);

	DWORD relAddr = (DWORD)hook - (DWORD)orig - 5;

	*(BYTE*)orig = 0xE9;
	orig = (LPVOID)((DWORD)orig + 1);
	*(DWORD*)orig = (DWORD)relAddr;


	if (!VirtualProtect(orig, HookObj->len, oldProt, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		return -1;
	}

	HookObj->isEnabled = true;
	return 1;
}

BOOL Trampo::DisableHook(PHOOK_OBJECT HookObj)
{
	if (!HookObj->isEnabled) {
		printf("hook is not enabled\n");
		return FALSE;
	}

	DWORD oldProt;

	if (!VirtualProtect(HookObj->original, HookObj->len, PAGE_EXECUTE_READWRITE, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}

	if (!RtlCopyMemory(HookObj->original, HookObj->stolenBytes, HookObj->len)) {
		printf("failed writing bytes: %d\n", GetLastError());
		exit(-1);
	}

	if (!VirtualProtect(HookObj->original, HookObj->len, oldProt, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}

	HookObj->isEnabled = false;
	return TRUE;
}

PHOOK_OBJECT Trampo::CreateHook(LPVOID hook, LPVOID orig, size_t len)
{
	PHOOK_OBJECT hookObj = (PHOOK_OBJECT)malloc(sizeof(HOOK_OBJECT));
	if (!hookObj) {
		printf("failed mallocing hook object\n");
		return 0;
	}

	RtlZeroMemory(hookObj, sizeof(HOOK_OBJECT));
	hookObj->len = len;
	hookObj->isEnabled = false;
	hookObj->hook = hook;
	hookObj->original = orig;

	BYTE* stolenBytes = (BYTE*)malloc(len);
	if (!stolenBytes) {
		printf("failed mallocing stolen bytes\n");
		return 0;
	}

	if (!RtlCopyMemory(stolenBytes, orig, len)) {
		printf("failed stealing bytes: %d\n", GetLastError());
		return 0;
	}

	hookObj->stolenBytes = stolenBytes;

	LPVOID tramp = VirtualAlloc(0, len + 5, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!tramp) {
		printf("failed VirtualAlloc: %d\n", GetLastError());
		return 0;
	}

	hookObj->trampoline = tramp;

	if (!RtlCopyMemory(tramp, stolenBytes, len)) {
		printf("failed writing bytes: %d\n", GetLastError());
		return 0;
	}

	DWORD retAddr = ((DWORD)orig + len) - ((DWORD)tramp + len + 5);

	tramp = (LPVOID)((DWORD)tramp + len);
	*(BYTE*)tramp = 0xE9;
	tramp = (LPVOID)((DWORD)tramp + 1);
	*(DWORD*)tramp = retAddr;

	return hookObj;
}

