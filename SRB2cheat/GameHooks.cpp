#include "GameHooks.h"
#include "main.h"
#include "offsets.h"

int RingDamageHook(int d, char e, int f) {

	printf("ur hit\n");

	return 1;
}

GameHooks::GameHooks()
{
	this->RingDamageObj = nullptr;
	this->RingDamageTrampoline = nullptr;
}

GameHooks::~GameHooks()
{
}

void GameHooks::init()
{
	this->RingDamageObj = Trampo::CreateHook(RingDamageHook, (LPVOID)RING_DMG_OFF, 5);
	if (!this->RingDamageObj) {
		exit(-1);
	}
	this->RingDamageTrampoline = (P_RingDamage)RingDamageObj->trampoline;
	return;
}