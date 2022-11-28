#include <Windows.h>
#include <iostream>
#include "trampy/Trampy.h"

#define ALL_EMERALDS 0x7f
#define RING_DMG_OFF 0x4FBDF0
#define KILL_PLAYER_OFF 0x4FC310


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

using P_KillPlayer = int (*)();
P_KillPlayer KillPlayerOrig;

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

    UINT_PTR base = (UINT_PTR)GetModuleHandle(0);
    RingDamageOrig = (P_RingDamage)RING_DMG_OFF;//((DWORD)(base + 0x163660));
    KillPlayerOrig = (P_KillPlayer)KILL_PLAYER_OFF;

    getCon();
    Player* player = (Player*)0x9E6D40;
    BYTE* emeralds = (BYTE*)0x9E55C0;

    PHOOK_DESCRIPTOR H_RingDamageHook = Trampy::CreateHook((LPVOID)RING_DMG_OFF, (LPVOID)RingDamageHook, (LPVOID*)&RingDamageOrig);
    PHOOK_DESCRIPTOR H_KillPlaterHook = Trampy::CreateHook((LPVOID)KILL_PLAYER_OFF, (LPVOID)KillPlayerHook, (LPVOID*)&KillPlayerOrig);

    while (1) {
        printf("rings: %d\t", player->rings);
        printf("emeralds %d\n", getBitNum(*emeralds));
        Sleep(1000);

        if (GetAsyncKeyState(VK_INSERT)) {
            Trampy::EnableHook(H_RingDamageHook);
            Trampy::EnableHook(H_KillPlaterHook);
            printf("god mode on");
        }

        if (GetAsyncKeyState(VK_END)) {
            *emeralds = ALL_EMERALDS;
            player->rings = 999;
            Trampy::EnableAllHooks();
        }
    }
    return 0;
}

int WINAPI main(HMODULE hModule) {

    if (!CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hack, hModule, NULL, NULL)) {
        printf("you stupid nigger\n, %d\n", GetLastError());
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
        /*if (!CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, hModule, NULL, NULL)) {
            printf("you stupid nigger\n, %d\n", GetLastError());
            return -1;
        }
        return 0;
        */
        main(hModule);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

