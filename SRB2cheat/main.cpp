#include "main.h"
#include "util.h"
#include "graphics.h"
#include "GameHooks.h"
#include "Trampoline.h"
#include "includes/d_player.h"
#include "includes/d_clisrv.h"

//offsets extracted from binary
player_t* player = (player_t*)PLAYER_OFF;
BYTE* emeralds = (BYTE*)EMERALD_OFF;

GameHooks* gameHooks = nullptr;
opengl* gl = nullptr;

//hooks
doomdata_t lastData;
PDWORD oldSendToIat = NULL;

//handles

int __stdcall sendToHook(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen) {
    if (buf) {
        doomdata_t* data = (doomdata_t*)buf;
        switch (data->packettype) {
        case PT_CLIENTJOIN:
        //case PT_CLIENTCMD:
            lastData = *data;
            break;
        }
    }
    return sendto(s, buf, len, flags, to, tolen);
}

void ImGuiWindow() {

    ImGui::Begin("SRB2Internal");

    ImGui::Separator();
    switch (lastData.packettype) {
    case PT_CLIENTCMD:
        ImGui::Text("Packet type: %d", lastData.packettype);
        ImGui::Text("aiming: %d", lastData.u.clientpak.cmd.aiming);
        ImGui::Text("angleturn: %d", lastData.u.clientpak.cmd.angleturn);
        ImGui::Text("buttons: %d", lastData.u.clientpak.cmd.buttons);
        ImGui::Text("forwardmove: %d", lastData.u.clientpak.cmd.forwardmove);
        ImGui::Text("latency: %d", lastData.u.clientpak.cmd.latency);
        ImGui::Text("sidemove: %d", lastData.u.clientpak.cmd.sidemove);
        break;
    
    case PT_CLIENTJOIN:
        //if (!online) online = !online;
        ImGui::Text("Packet type: %d", lastData.packettype);
        ImGui::Text("application: %s", lastData.u.clientcfg.application);
        ImGui::Text("localplayers: %d", lastData.u.clientcfg.localplayers);
        ImGui::Text("mode: %d", lastData.u.clientcfg.mode);
        ImGui::Text("modversion: %d", lastData.u.clientcfg.modversion);
        for(int i = 0; lastData.u.clientcfg.names[i][0]; i++)
        ImGui::Text("names: %s", lastData.u.clientcfg.names[i]);
        break;
    }

    ImGui::SliderShort("Rings", &player[triggers::playernum].rings, 0, 32767, "%d");
    ImGui::SliderUByte("acceleration", &player[triggers::playernum].acceleration, 0, 255, "%d");
    ImGui::SliderInt("score", (int*) & player[triggers::playernum].score, 0, 2147483647, "%d");
    ImGui::SliderInt("dashspeed", &triggers::superDash, 0, 2147483647, "%d");
    ImGui::SliderInt("normal speed", &player[triggers::playernum].normalspeed, 0, 2147483647, "%d");
    ImGui::SliderInt("run speed", &player[triggers::playernum].runspeed, 0, 2147483647, "%d");
    ImGui::SliderUByte("accelstart", &player[triggers::playernum].accelstart, 0, 255, "%d");
    ImGui::SliderUByte("thrustfactor", &player[triggers::playernum].thrustfactor, 0, 255, "%d");
    ImGui::InputInt("playernum", &triggers::playernum, 1, 100, 0);
    ImGui::Checkbox("all emeralds", &triggers::allEmeralds);
    ImGui::Checkbox("God Mode", &triggers::godMode);
    ImGui::Checkbox("Infinite Thok", &triggers::infiniteThok);
    ImGui::Checkbox("Always Super", &triggers::alwaysSuper);
    ImGui::End();
}

int hack(HMODULE hModule) {

    gl = new opengl();
    gameHooks = new GameHooks();
    gameHooks->init();
    gl->init();
    gl->activate(gl->wglSwapBuffersObj);

    *(DWORD*)sendToIat = (DWORD)sendToHook; //TODO: parse IAT properly

    Util::getCon();

    return 0;
}

void hackLoop() {

    if ((DWORD)player != (DWORD)PLAYER_OFF + (32* triggers::playernum))
        player = (player_t*)((DWORD)PLAYER_OFF + (32 * triggers::playernum));

    if (triggers::superDash)
        player[triggers::playernum].dashspeed = triggers::superDash;

    if (triggers::alwaysSuper)
        if (!player[triggers::playernum].powers[pw_super])
            player[triggers::playernum].powers[pw_super]++;

    if (triggers::infiniteThok)
        if (player[triggers::playernum].pflags & PF_THOKKED)
            player[triggers::playernum].pflags = (pflags_t)(player[triggers::playernum].pflags ^ PF_THOKKED);

    if (ImGui::IsKeyPressed(ImGuiKey_Insert))
        triggers::godMode = !triggers::godMode;

    if (triggers::godMode) {
        if (!gameHooks->RingDamageObj->isEnabled)
            gameHooks->activate(gameHooks->RingDamageObj);
        if (player[triggers::playernum].rings < 2) player[triggers::playernum].rings = 2;
    }
    else {
        if (gameHooks->RingDamageObj->isEnabled)
            gameHooks->deactivate(gameHooks->RingDamageObj);
    }

    if (triggers::allEmeralds)
        *emeralds = 0x7f;
    else {
        *emeralds = 0;
    }
}

int WINAPI main(HMODULE hModule) {

    /*if (!CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hack, hModule, NULL, NULL)) {
        printf("failed CreateThread\n, %d\n", GetLastError());
        return -1;
    }*/
    hack(hModule);

    return 0;
}