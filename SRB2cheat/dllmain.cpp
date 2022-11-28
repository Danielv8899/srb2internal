#include <Windows.h>
#include <iostream>
#include "Trampoline.h"

#define ALL_EMERALDS 0x7f
#define RING_DMG_OFF 0x4FBDF0
#define KILL_PLAYER_OFF 0x4FC310
#define PLAYER_OFF 0x9E6D40
#define EMERALD_OFF 0x9E55C0


struct Player {
    UINT_PTR ptr;
    BYTE fwBk;
    BYTE RightLeft;
    short CamPos;
    short padding;
    BYTE isSpin;
    BYTE isJump;
    BYTE isPaused;
    BYTE padding1 [0xf];
    short slope;
    short relativeHight;
    BYTE padding2[0x10];
    unsigned int CamPos2;
    BYTE padding3[0x5];
    short CamPos3;
    short rings;

};

using P_RingDamage = int (* )();
P_RingDamage RingDamageOrig;

//DO NOT USE
//using P_KillPlayer = int (*)();
//P_KillPlayer KillPlayerOrig;

int KillPlayerHook() {
    return 1;
}

int RingDamageHook() {
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

    RingDamageOrig = (P_RingDamage)RING_DMG_OFF;
    //KillPlayerOrig = (P_KillPlayer)KILL_PLAYER_OFF;

    getCon();
    Player* player = (Player*)PLAYER_OFF;
    BYTE * emeralds = (BYTE*)EMERALD_OFF;

    BYTE* RingDamageStolenBytes = NULL;
    LPVOID RingTrampoline = NULL;

    while (1) {
        printf("rings: %d\t", player->rings);
        printf("emeralds %d\n", getBitNum(*emeralds));
        Sleep(1000);

        if (GetAsyncKeyState(VK_INSERT)) {

            Trampo::EnableHook(RingDamageHook,RingDamageOrig ,&RingTrampoline, &RingDamageStolenBytes);
            printf("god mode on\n");
        }

        if (GetAsyncKeyState(VK_HOME)) {
            Trampo::DisableHook(RingDamageOrig, RingTrampoline, RingDamageStolenBytes);
            printf("god mode off\n");
        }

        if (GetAsyncKeyState(VK_END)) {
            *emeralds = ALL_EMERALDS;
            player->rings = 999;
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

