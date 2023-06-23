#pragma once
#include "HookObj.h"

using P_RingDamage = int (*)(int, char, int);

class GameHooks :public HookObj
{
	public:
		GameHooks();
		~GameHooks();
		void init();

		PHOOK_OBJECT RingDamageObj;
		P_RingDamage RingDamageTrampoline;
};

int RingDamageHook(int d, char e, int f);

