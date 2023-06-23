#include "network.h"

doomdata_t lastData = { 0 };
PDWORD oldSendToIat = NULL;

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