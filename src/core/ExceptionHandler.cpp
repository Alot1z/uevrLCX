#ifdef _WIN32
#include <windows.h>
#include "uevr/ExceptionHandler.hpp"

LONG WINAPI framework::global_exception_handler(struct _EXCEPTION_POINTERS* ei) {
    // Simple exception handler for minimal build
    return EXCEPTION_CONTINUE_SEARCH;
}

void framework::setup_exception_handler() {
    SetUnhandledExceptionFilter(global_exception_handler);
}
<<<<<<< Current (Your changes)
=======
#else
#include "ExceptionHandler.hpp"

// Non-Windows platforms: no-op implementation
namespace framework {
void setup_exception_handler() {}
}
#endif
>>>>>>> Incoming (Background Agent changes)
