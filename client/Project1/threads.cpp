#include "threads.h"

void threads::antidebug_thread() {
    LPCSTR funcs[] = { xorstr_("IsDebuggerPresent"), xorstr_("CheckRemoteDebuggerPresent"), xorstr_("ExitProcess"), xorstr_("OpenThread"), xorstr_("SuspendThread"), xorstr_("ResumeThread"), xorstr_("CloseHandle"), xorstr_("OpenProcess"), xorstr_("CreateRemoteThread"), xorstr_("WriteProcessMemory"), xorstr_("VirtualProtect"), xorstr_("VirtualProtectEx"), xorstr_("LoadLibraryA"), xorstr_("LoadLibraryExA"), xorstr_("LoadLibraryW"), xorstr_("LoadLibraryExW") };

    time_t curtime = time(0);
    auto expires = curtime + 120;

    while (!done) {
        for (LPCSTR func : funcs) {
            if (loader::is_func_hooked(LoadLibrary(xorstr_(L"kernel32.dll")), func)) {
                done = true;
                LI_FN(ExitProcess)(0);
            }
        }

        if (LI_FN(IsDebuggerPresent)()) {
            done = true;
            LI_FN(ExitProcess)(0);
        }

        BOOL remote_debugger;
        LI_FN(CheckRemoteDebuggerPresent)(LI_FN(GetCurrentProcess)(), &remote_debugger);

        if (remote_debugger) {
            done = true;
            LI_FN(ExitProcess)(0);
        }

        curtime = time(0);
        remaining = expires - curtime;

        if (curtime >= expires) {
            done = true;
            LI_FN(ExitProcess)(0);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void threads::gui_thread() {
    gui::init();
}