#pragma once
#include <winsock.h>
#include "includes/d_clisrv.h"

int __stdcall sendToHook(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);
extern doomdata_t lastData;
extern PDWORD oldSendToIat;

class network
{
};

