#include <Windows.h>
#include <iostream>
#include "Trampoline.h"

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

//using SendNetXCmd = int(__cdecl*)(char, void*, size_t);
//SendNetXCmd SendNetXCmdOrig;

/*int __cdecl SendNetXCmdHook(char a1, void* a2, size_t a3) {
    printf("SendNetXCmd Called\nid: %d\nnparam: %d\nparam: ", a1, a3);
    for (int i = 0; i < a3; i++) {
        if (i > 0) printf(":");
        printf("%02X", ((char*)a2)[i]);
    }
    printf("\n----------------------\n");
    return(SendNetXCmdOrig(a1, a2, a3));
}*/

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
    //SendNetXCmdOrig = (SendNetXCmd)0x43ABC0;

    getCon();
    Player* player = (Player*)PLAYER_OFF;
    BYTE * emeralds = (BYTE*)EMERALD_OFF;
    BYTE* runSpeed = (BYTE*)(CHAR_SPEED + 2);
    BYTE* normalSpeed = (BYTE*)(CHAR_SPEED + 6);
    BYTE* CharAccel = (BYTE*)CHAR_ACCELERATION;
    BYTE* jumpForce = (BYTE*)(ACTION_SPEED + 14);

    PHOOK_OBJECT RingDamageObj = Trampo::CreateHook(RingDamageHook, RingDamageOrig);
    //PHOOK_OBJECT SendNetXObj = Trampo::CreateHook(SendNetXCmdHook, SendNetXCmdOrig);

    while (1) {
        
        printf("rings: %d\t", player->rings);
        printf("emeralds %d\n", getBitNum(*emeralds));
        printf("normalSpeed %d\n",  *normalSpeed);
        printf("runSpeed %d\n", *runSpeed);
        printf("actionspeed %d\n", *jumpForce);
        printf("thrustfactor: %hhx\taccelstart: %hhx\tacceleration: %hhx\n", CharAccel[0], CharAccel[1], CharAccel[2]);

        Sleep(1000);

        if (GetAsyncKeyState(VK_INSERT)) {

            if(Trampo::EnableHook(RingDamageObj))
                printf("god mode on\n");
            //Trampo::EnableHook(SendNetXObj);
        }

        if (GetAsyncKeyState(VK_HOME)) {
            if(Trampo::DisableHook(RingDamageObj))
                printf("god mode off\n");
            //Trampo::DisableHook(SendNetXObj);
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

