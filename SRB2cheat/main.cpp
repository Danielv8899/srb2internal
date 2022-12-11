#include "main.h"
#include "util.h"

//offsets extracted from binary
player_t* player = (player_t*)PLAYER_OFF;
BYTE* emeralds = (BYTE*)EMERALD_OFF;

//hooks
PHOOK_OBJECT RingDamageObj = NULL;
doomdata_t lastData;
P_RingDamage RingDamageTrampoline;
extern "C" wglSwapBuffers_t wglSwapBuffersTrampoline = nullptr;
PDWORD oldSendToIat = NULL;

//handles
HWND hGameWindow;
WNDPROC hGameWindowProc;

//ImGui triggers
bool godMode = false;
bool allEmeralds = false;
bool infiniteThok = false;
bool alwaysSuper = false;
bool menuShown = true;
int superDash = 0;

LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE) {
        menuShown = !menuShown;
        return false;
    }

    if (menuShown) {
        CallWindowProc((WNDPROC)ImGui_ImplWin32_WndProcHandler, hWnd, uMsg, wParam, lParam);
    }
    return CallWindowProc(hGameWindowProc, hWnd, uMsg, wParam, lParam);
}

extern "C" void __stdcall RenderHook(_In_ HDC hDc) {

    static bool imGuiInitialized = false;
    if (!imGuiInitialized) {
        imGuiInitialized = true;

        hGameWindow = WindowFromDC(hDc);

        if (!hGameWindowProc)
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

    if (menuShown) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGuiWindow();
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    hackLoop();
    wglSwapBuffersTrampoline(hDc);
}

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

void ImGuiWindow() {

    ImGui::Begin("SRB2Internal");

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

    ImGui::SliderShort("Rings", &player->rings, 0, 32767, "%d");
    ImGui::SliderUByte("acceleration", &player->acceleration, 0, 255, "%d");
    ImGui::SliderInt("score", (int*) & player->score, 0, 2147483647, "%d");
    ImGui::SliderInt("dashspeed", &superDash, 0, 2147483647, "%d");
    ImGui::SliderInt("normal speed", &player->normalspeed, 0, 2147483647, "%d");
    ImGui::SliderInt("run speed", &player->runspeed, 0, 2147483647, "%d");
    ImGui::SliderUByte("accelstart", &player->accelstart, 0, 255, "%d");
    ImGui::SliderUByte("thrustfactor", &player->thrustfactor, 0, 255, "%d");
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

    Util::getCon();


    return 0;
}

void hackLoop() {

    if (superDash)
        player->dashspeed = superDash;

    if (alwaysSuper)
        if (!player->powers[pw_super])
            player->powers[pw_super]++;

    if (infiniteThok)
        if (player->pflags & PF_THOKKED)
            player->pflags = (pflags_t)(player->pflags ^ PF_THOKKED);

    if (ImGui::IsKeyPressed(ImGuiKey_Insert))
        godMode = !godMode;

    if (godMode) {
        if (!RingDamageObj->isEnabled)
            Trampo::EnableHook(RingDamageObj);
        if (player->rings < 2) player->rings = 2;
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
}

int WINAPI main(HMODULE hModule) {

    if (!CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hack, hModule, NULL, NULL)) {
        printf("failed CreateThread\n, %d\n", GetLastError());
        return -1;
    }

    return 0;
}