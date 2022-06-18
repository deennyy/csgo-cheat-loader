#pragma once

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>

#include "xorstr.h"
#include "li_fn.h"
#include "loader.h"
#include "http.h"
#include "hwid.h"

#include <TlHelp32.h>
#include <Psapi.h>
#include <time.h>
#include <iomanip>
#include <sstream>

namespace gui {
	void init();
}

extern bool done;
extern int remaining;
extern std::string login_token;
extern std::string sub_status;