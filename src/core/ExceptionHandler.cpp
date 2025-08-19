#include <windows.h>
#include "ExceptionHandler.hpp"

LONG WINAPI framework::global_exception_handler(struct _EXCEPTION_POINTERS* ei) {
    // Simple exception handler for minimal build
    return EXCEPTION_CONTINUE_SEARCH;
}

void framework::setup_exception_handler() {
    SetUnhandledExceptionFilter(global_exception_handler);
}
