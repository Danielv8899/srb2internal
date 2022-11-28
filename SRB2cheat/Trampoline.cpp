#include "Trampoline.h"

void Trampo::EnableHook(LPVOID hook, LPVOID orig, LPVOID * trampoline , PBYTE* stolen) {

	BYTE stolenBytes[5] = { 0 };

	DWORD oldProt;

	if (!VirtualProtect(orig, 0x5, PAGE_EXECUTE_READWRITE, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}

	if (!RtlCopyMemory(stolenBytes, orig, 5)) {
		printf("failed stealing bytes: %d\n", GetLastError());
		exit(-1);
	}

	*stolen = stolenBytes;

	LPVOID tramp = VirtualAlloc(0, 10, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!tramp) {
		printf("failed VirtualAlloc: %d\n", GetLastError());
		exit(-1);
	}

	*trampoline = tramp;

	if (!RtlCopyMemory(tramp, stolenBytes, 5)) {
		printf("failed writing bytes: %d\n", GetLastError());
		exit(-1);
	}

	DWORD relAddr = (DWORD)hook - (DWORD)orig - 5;
	DWORD retAddr = ((DWORD)tramp + 5) - ((DWORD)orig + 5) - 5;

	tramp = (LPVOID)((DWORD)tramp + 5);
	*(BYTE*)tramp = 0xE9;
	tramp = (LPVOID)((DWORD)tramp + 1);
	*(DWORD*)tramp = retAddr;

	*(BYTE*)orig = 0xE9;
	orig = (LPVOID)((DWORD)orig + 1);
	*(DWORD*)orig = (DWORD)relAddr;

	if (!VirtualProtect(orig, 0x5, oldProt, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}
}

void Trampo::DisableHook(LPVOID orig, LPVOID trampoline, PBYTE stolenBytes)
{
	DWORD oldProt;

	if (!VirtualProtect(orig, 0x5, PAGE_EXECUTE_READWRITE, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}

	if (!RtlCopyMemory(orig, stolenBytes, 5)) {
		printf("failed writing bytes: %d\n", GetLastError());
		exit(-1);
	}

	if (!VirtualFree(trampoline, 0, MEM_RELEASE)) {
		printf("failed freeing trampoline: %d\n", GetLastError());
		exit(-1);
	}

	if (!VirtualProtect(orig, 0x5, oldProt, &oldProt)) {
		printf("failed VirtualProtect: %d\n", GetLastError());
		exit(-1);
	}
}

