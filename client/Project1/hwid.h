#pragma once

#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>
#include <vector>
#include <string>

#include "xorstr.h"
#include "li_fn.h"
#include "sha256.h"

namespace hwid {
	std::string get_hwid();
}