#pragma once
#ifndef INT
#include <windows.h>
#endif // !INT

namespace Util {
	//attach console to process
	void getCon();

	//get amount of turned on bits in byte
	INT getBitNum(BYTE byte);
}