#pragma once

#include <Windows.h>
#include <winhttp.h>

#include <string>
#include <vector>

#include "li_fn.h"
#include "xorstr.h"

namespace http {
	std::string get_binary_data(const wchar_t url[]);
	std::string get_api_data(const wchar_t url[]);
}