#ifdef _WIN32
#include <windows.h>
#include "ExceptionHandler.hpp"

LONG WINAPI framework::global_exception_handler(struct _EXCEPTION_POINTERS* ei) {
    // Simple exception handler for minimal build
    (void)ei; // Suppress unused parameter warning
    return EXCEPTION_CONTINUE_SEARCH;
}

void framework::setup_exception_handler() {
    SetUnhandledExceptionFilter(global_exception_handler);
}
#else
#include "ExceptionHandler.hpp"

// Non-Windows platforms: no-op implementation
namespace framework {
void setup_exception_handler() {}
}
#endif
