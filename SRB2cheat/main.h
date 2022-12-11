#pragma once
#include <Windows.h>
#include <iostream>
#include "Trampoline.h"
#include <winsock.h>
#include "includes/d_player.h"
#include "includes/d_clisrv.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/glew.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(lib,"Ws2_32.lib")

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
using P_RingDamage = int (*)(int, char, int);
using wglSwapBuffers_t = void(__stdcall*)(_In_ HDC hDc);

//hook signatures;

//wglSwapBuffers hook
extern "C" void __stdcall RenderHook(_In_ HDC hDc);
//WS_32 sendTo hook
int __stdcall sendToHook(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);
//this function is called when a player gets hit and has more than 0 rings
int RingDamageHook(int , char , int );
//wndProc hook
LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//wndProc default handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);