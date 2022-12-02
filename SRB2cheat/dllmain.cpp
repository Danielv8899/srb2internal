#include <Windows.h>
#include <iostream>
#include "Trampoline.h"
#include <winsock.h>
#include "includes/d_player.h"
#include "includes/d_clisrv.h"

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

using HWR_RenderWall = int(__cdecl*)(void* a1, BYTE* a2, int a3, int a4, int a5, int a6);
HWR_RenderWall RenderWallTrampoline;

using P_RingDamage = int (* )(int , char, int);
P_RingDamage RingDamageTrampoline;

int __cdecl HWR_RenderWallHook(void* a1, BYTE* a2, int a3, int a4, int a5, int a6) {
    __asm pushad;
    printf("HWR_RenderWall called\n");
    __asm popad;
    return RenderWallTrampoline(a1, a2, a3, a4, a5, a6);
}

int __stdcall sendToHook(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen) {
    if (buf) {
        doomdata_t* data = (doomdata_t*)buf;
        switch (data->packettype) {
        case PT_CLIENTCMD:
            printf("aiming: %d\n", data->u.clientpak.cmd.aiming);
            printf("angleturn: %d\n", data->u.clientpak.cmd.angleturn);
            printf("buttons: %d\n", data->u.clientpak.cmd.buttons);
            printf("forwardmove: %d\n", data->u.clientpak.cmd.forwardmove);
            printf("latency: %d\n", data->u.clientpak.cmd.latency);
            printf("sidemove: %d\n", data->u.clientpak.cmd.sidemove);
            printf("\n----------------------\n");
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

int hack(HMODULE hModule) {

    //doomcom = (pdoomcom_t)0x9AEEB8;
    Trampo Trampo;
    

    getCon();

    player_t* player = (player_t*)PLAYER_OFF;
    BYTE * emeralds = (BYTE*)EMERALD_OFF;
    BYTE* runSpeed = (BYTE*)(CHAR_SPEED + 2);
    BYTE* normalSpeed = (BYTE*)(CHAR_SPEED + 6);
    BYTE* CharAccel = (BYTE*)CHAR_ACCELERATION;
    BYTE* jumpForce = (BYTE*)(ACTION_SPEED + 14);

    auto RingDamageObj = Trampo.CreateHook(RingDamageHook, (LPVOID)RING_DMG_OFF,5);
    auto RenderWallObj = Trampo.CreateHook(HWR_RenderWallHook, (LPVOID)0x420B90, 7);

    RingDamageTrampoline = (P_RingDamage)RingDamageObj->trampoline;
    RenderWallTrampoline = (HWR_RenderWall)RenderWallObj->trampoline;

    *(DWORD*)sendToIat = (DWORD)sendToHook;

    while (1) {

        //printf("dashspeed %d\n", player->dashspeed);
        if(player->pflags & PF_THOKKED)
        player->pflags = (pflags_t)(player->pflags ^ PF_THOKKED);
        //printf("suepr: %d", player->powers[pw_super]);

        //if (!player->powers[pw_super])
        //    player->powers[pw_super]++;

        if (GetAsyncKeyState(VK_INSERT)) {
            if(Trampo.EnableHook(RingDamageObj.get()))
                printf("god mode on\n");
            //Trampo::EnableHook(RenderWallObj);
        }

        if (GetAsyncKeyState(VK_HOME)) {
            if(Trampo.DisableHook(RingDamageObj.get()))
                printf("god mode off\n");
            //Trampo::DisableHook(RenderWallObj);
        }

        if (GetAsyncKeyState(VK_END)) {
            *emeralds = ALL_EMERALDS;
            player->rings = 999;
            player->acceleration = 0xff;
            player->dashspeed = 0xffff;
            player->normalspeed = 0xff;
            
        }
    }
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

