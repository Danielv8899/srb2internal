#include <windows.h>
#include <iostream>
#include <string>
#pragma comment (lib,"User32.lib")
#include "Player.h"

Player* player;
uintptr_t baseAddr;


DWORD WINAPI Thread(HMODULE hModule) {
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	baseAddr = (uintptr_t)GetModuleHandle(0);
	player = (Player*)(baseAddr + 0x5e6d40);
	while (true) {
		
		if (GetAsyncKeyState(VK_END)) {
			break;
		}
		if (GetAsyncKeyState(VK_INSERT)) {
			player->rings =999;
		}
		
		std::cout << "KeyPressedLeftRight: " << player->KeyPressedLeftRight << std::endl;
		std::cout << "KeyPressedUpDown: " << player->KeyPressedUpDown << std::endl;
		std::cout << "camAngle: " << player->camAngle << std::endl;
		std::cout << "isJumping: " << (player->isJumping == 8) << std::endl;
		std::cout << "isJumping2: " << !(player->isJumping2 == 4) << std::endl;
		std::cout << "yPos: " << player->yPos << std::endl;
		std::cout << "isSpinning: " << (int)player->isSpinning << std::endl;
		std::cout << "isPaused: " << (int)player->isPaused << std::endl;
		std::cout << "velocity: " << player->Velocity << std::endl;
		std::cout << "rings: " << player->rings << std::endl;
		std::cout << "time: " << player->time << std::endl;
		std::cout << "-----------------" << std::endl;
		Sleep(2000);
	}
	return 0;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		HANDLE hThread = 0;
		hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Thread, hModule, 0, 0);
		if (hThread) {
			CloseHandle(hThread);
		}
	}
		break;
	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}
