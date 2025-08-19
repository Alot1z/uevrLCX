#include <windows.h>
#include <iostream>
#include "uevr/ExceptionHandler.hpp"

int main() {
    std::cout << "UEVR Core Test Starting..." << std::endl;
    
    // Test exception handler
    framework::global_exception_handler(nullptr);
    
    std::cout << "UEVR Core Test Completed Successfully!" << std::endl;
    return 0;
}
