#pragma once
#include <Windows.h>
#include <iostream>
#include <winsock.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/glew.h"
#include "imgui/imgui_impl_win32.h"
#pragma comment(lib,"Ws2_32.lib")

namespace triggers {
	bool godMode = 0;
	bool allEmeralds = 0;
	bool infiniteThok = 0;
	bool alwaysSuper = 0;
	bool menuShown = 0;
	int superDash = 0;
	std::string playername;
	int playernum = 0;
};

//offsets extracted from binary
#define ALL_EMERALDS 0x7f
#define RING_DMG_OFF 0x4FBDF0
#define KILL_PLAYER_OFF 0x4FC310
#define PLAYER_OFF 0x9E6D40 //9e6fac
#define EMERALD_OFF 0x9E55C0
#define LOAD_MODEL 0x42B8D0
#define SERVER 0x66BC20
#define LOCAL_TEXT_CMD 0x009AD860
#define sendToIat 0x5463D28

//signatures

//imGui window
void ImGuiWindow();
//setup hooks and enter render loop
int hack(HMODULE hModule);
//used to update memory using imgui triggers
void hackLoop();
//opens hack thread
int WINAPI main(HMODULE hModule);

//extracted signatures from binary

//hook signatures;

//wglSwapBuffers hook
//WS_32 sendTo hook
int __stdcall sendToHook(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);
//this function is called when a player gets hit and has more than 0 rings
//wndProc hook//wndProc default handler