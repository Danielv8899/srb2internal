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

#define ALL_EMERALDS 0x7f
#define RING_DMG_OFF 0x4FBDF0
#define KILL_PLAYER_OFF 0x4FC310
#define PLAYER_OFF 0x9E6D40
#define EMERALD_OFF 0x9E55C0
#define CHAR_SPEED 0x9E6DF4
#define CHAR_ACCELERATION 0x9E6DFC
#define ACTION_SPEED 0x9E6E1C
#define LOAD_MODEL 0x42B8D0
#define SERVER 0x66BC20
#define LOCAL_TEXT_CMD 0x009AD860
#define sendToIat 0x5463D28

HWND hGameWindow;
WNDPROC hGameWindowProc;
bool menuShown = true;

void ImGuiWindow();

using P_RingDamage = int (* )(int , char, int);
P_RingDamage RingDamageTrampoline;

using wglSwapBuffers_t = void(__stdcall*)(_In_ HDC hDc);

extern "C" wglSwapBuffers_t wglSwapBuffersTrampoline = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE) {
        menuShown = !menuShown;
        return false;
    }

    if (menuShown) {
        CallWindowProc((WNDPROC)ImGui_ImplWin32_WndProcHandler, hWnd, uMsg, wParam, lParam);
    }
    return CallWindowProc(hGameWindowProc,hWnd, uMsg, wParam, lParam);
}

extern "C" void __stdcall RenderHook(_In_ HDC hDc) {

    static bool imGuiInitialized = false;
    if (!imGuiInitialized) {
        imGuiInitialized = true;

        hGameWindow = WindowFromDC(hDc);

        if(!hGameWindowProc)
        hGameWindowProc = (WNDPROC)SetWindowLongPtr(hGameWindow,
            GWLP_WNDPROC, (LONG_PTR)windowProc_hook);

        glewInit();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hGameWindow);
        ImGui_ImplOpenGL3_Init();
        ImGui::StyleColorsDark();
        ImGui::GetStyle().AntiAliasedFill = false;
        ImGui::GetStyle().AntiAliasedLines = false;
        ImGui::CaptureMouseFromApp();
        ImGui::CaptureKeyboardFromApp();
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
    }

    // If the menu is shown, start a new frame and draw the demo window
    if (menuShown) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGuiWindow();
        ImGui::EndFrame();
        ImGui::Render();

        // Draw the overlay
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    wglSwapBuffersTrampoline(hDc);
}

doomdata_t lastData;

int __stdcall sendToHook(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen) {
    if (buf) {
        doomdata_t* data = (doomdata_t*)buf;
        switch (data->packettype) {
        case PT_CLIENTCMD:
            lastData = *data;
            break;
        }
    }
    return sendto(s, buf, len, flags, to, tolen);
}

int RingDamageHook(int d, char e, int f) {

    printf("ur hit\n");

    return 1;
}

INT getBitNum(BYTE byte) {
    BYTE nibble_lookup[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

    return nibble_lookup[byte & 0x0F] + nibble_lookup[byte >> 4];
}

void getCon() {

    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    (void)freopen_s((FILE**)stdout, "CON", "w", stdout);
    (void)freopen_s((FILE**)stdin, "CON", "r", stdin);

}

player_t* player = (player_t*)PLAYER_OFF;
BYTE* emeralds = (BYTE*)EMERALD_OFF;
BYTE* runSpeed = (BYTE*)(CHAR_SPEED + 5);
BYTE* normalSpeed = (BYTE*)(CHAR_SPEED + 6);
BYTE* CharAccel = (BYTE*)CHAR_ACCELERATION;
BYTE* jumpForce = (BYTE*)(ACTION_SPEED + 14);

PHOOK_OBJECT RingDamageObj = NULL;

bool godMode = false;
bool allEmeralds = false;
bool infiniteThok = false;
bool alwaysSuper = false;


PDWORD oldSendToIat = NULL;

void ImGuiWindow() {

    ImGui::Begin("Dear ImGui Demo");
    
        ImGui::Text("Packet type: %d", lastData.packettype);
        ImGui::Separator();
        switch (lastData.packettype) {
        case PT_CLIENTCMD:
            ImGui::Text("aiming: %d", lastData.u.clientpak.cmd.aiming);
            ImGui::Text("angleturn: %d", lastData.u.clientpak.cmd.angleturn);
            ImGui::Text("buttons: %d", lastData.u.clientpak.cmd.buttons);
            ImGui::Text("forwardmove: %d", lastData.u.clientpak.cmd.forwardmove);
            ImGui::Text("latency: %d", lastData.u.clientpak.cmd.latency);
            ImGui::Text("sidemove: %d", lastData.u.clientpak.cmd.sidemove);
            break;
        }

        if(alwaysSuper)
            if (!player->powers[pw_super])
                player->powers[pw_super]++;

        if(infiniteThok)
            if (player->pflags & PF_THOKKED)
                player->pflags = (pflags_t)(player->pflags ^ PF_THOKKED);

        if (ImGui::IsKeyPressed(ImGuiKey_Insert))
            godMode = !godMode;

            if (godMode) {
                if (!RingDamageObj->isEnabled)
                    Trampo::EnableHook(RingDamageObj);
            }
            else {
                if (RingDamageObj->isEnabled)
                    Trampo::DisableHook(RingDamageObj);
            }

            if (allEmeralds)
                *emeralds = 0x7f;
            else {
                *emeralds = 0;
            }
    
    ImGui::SliderInt("Rings", (int*)&player->rings, 0, 32767, "%d");
    ImGui::SliderInt("acceleration", (int*)&player->acceleration, 0, 255, "%d");
    ImGui::SliderInt("dashspeed", (int*)&player->dashspeed, 0, 4294967295, "%d");
    ImGui::SliderInt("normal speed", (int*)normalSpeed, 0, 255, "%d");
    ImGui::SliderInt("run speed", (int*)runSpeed, 0, 255, "%d");
    ImGui::Checkbox("all emeralds", &allEmeralds);
    ImGui::Checkbox("God Mode", &godMode);
    ImGui::Checkbox("Infinite Thok", &infiniteThok);
    ImGui::Checkbox("Always Super", &alwaysSuper);

    ImGui::End();
}

int hack(HMODULE hModule) {

    DWORD wglSwapBuffersHookAddr = NULL;
    HMODULE hOpengl32 = GetModuleHandle(L"opengl32.dll");
    if (hOpengl32 != nullptr) {
        wglSwapBuffersHookAddr = (DWORD)GetProcAddress(hOpengl32, "wglSwapBuffers");
    }
    else { 
        printf("failed getting swapbuffer\n");
        exit(-1);
    }
    *(DWORD*)sendToIat = (DWORD)sendToHook;

    RingDamageObj = Trampo::CreateHook(RingDamageHook, (LPVOID)RING_DMG_OFF, 5);

    RingDamageTrampoline = (P_RingDamage)RingDamageObj->trampoline;

    auto wglSwapBuffersObj = Trampo::CreateHook(RenderHook, (LPVOID)wglSwapBuffersHookAddr, 5);
    wglSwapBuffersTrampoline = (wglSwapBuffers_t)wglSwapBuffersObj->trampoline;
    Trampo::EnableHook(wglSwapBuffersObj);

    getCon();
   
    return 0;
}

int WINAPI main(HMODULE hModule) {

    if (!CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hack, hModule, NULL, NULL)) {
        printf("failed CreateThread\n, %d\n", GetLastError());
        return -1;
    }
    
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        main(hModule);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

