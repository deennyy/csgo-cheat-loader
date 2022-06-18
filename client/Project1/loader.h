#pragma once

#include "../../Blackbone-master/src/BlackBone/Process/Process.h"
#include "http.h"
#include "xorstr.h"
#include "gui.h"

#include "../../cryptopp860/modes.h"
#include "../../cryptopp860/aes.h"
#include "../../cryptopp860/filters.h"
#include "../../cryptopp860/hex.h"

namespace loader {
	bool inject();
	bool is_func_hooked(HMODULE Module, LPCSTR Function);
	BOOL vm_check();
	bool check_testsigning();
}