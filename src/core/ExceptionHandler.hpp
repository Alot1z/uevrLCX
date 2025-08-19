#ifdef _WIN32
#include <windows.h>
#else
// Define dummy types for non-Windows builds
using HMODULE = void*;
using DWORD = unsigned long;
#endif

namespace framework {
#ifdef _WIN32
LONG WINAPI global_exception_handler(struct _EXCEPTION_POINTERS* ei);
void setup_exception_handler();
#else
inline void setup_exception_handler() {}
#endif
}
