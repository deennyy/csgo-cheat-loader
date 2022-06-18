#pragma once

#include <thread>

#include <Windows.h>

#include "li_fn.h"

#include "gui.h"

namespace threads {
	void antidebug_thread();
	void gui_thread();
}