#include "ExceptionHandler.hpp"
#include <spdlog/spdlog.h>

void uevr::handleException(const std::exception& e) {
    spdlog::error("Exception caught: {}", e.what());
}

void uevr::handleException(const std::string& message) {
    spdlog::error("Exception caught: {}", message);
}

void uevr::handleException(const char* message) {
    spdlog::error("Exception caught: {}", message);
}
