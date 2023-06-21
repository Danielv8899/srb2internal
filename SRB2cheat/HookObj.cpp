#include "HookObj.h"

void HookObj::activate(PHOOK_OBJECT obj)
{
	int res = Trampo::EnableHook(obj);
	switch (res) {
	case -1:
		exit(-1);

	default:
		return;
	}
}

void HookObj::deactivate(PHOOK_OBJECT obj)
{
	int res = Trampo::DisableHook(obj);
	switch (res) {
	case -1:
		exit(-1);

	default:
		return;
	}
}
