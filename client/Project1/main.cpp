#include "http.h"
#include "threads.h"
#include "loader.h"
#include "hwid.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
#ifndef DEBUG
    std::thread debug_thread(threads::antidebug_thread);
#endif // !DEBUG
    
    if (loader::vm_check() == TRUE) {
        done = true;
        LI_FN(ExitProcess)(0);
    }

    if (loader::check_testsigning() == true) {
        done = true;
        LI_FN(ExitProcess)(0);
    }

    std::thread gui_thread(threads::gui_thread);

#ifndef DEBUG
    debug_thread.join();
#endif // !DEBUG

    gui_thread.join();
}