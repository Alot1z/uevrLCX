#include "uevr/Plugin.hpp"
#include "uevr/Framework.hpp"
#include <iostream>

namespace uevr {

Plugin::Plugin() : m_framework(nullptr), m_isInitialized(false) {
    std::cout << "uevr Plugin constructor called" << std::endl;
}

Plugin::~Plugin() {
    if (m_isInitialized) {
        cleanup();
    }
}

bool Plugin::initialize() {
    try {
        std::cout << "Initializing uevr Plugin..." << std::endl;
        
        // Create framework instance
        m_framework = std::make_unique<Framework>();
        
        if (!m_framework->initialize()) {
            std::cerr << "Failed to initialize uevr framework" << std::endl;
            return false;
        }
        
        m_isInitialized = true;
        std::cout << "uevr Plugin initialized successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during plugin initialization: " << e.what() << std::endl;
        return false;
    }
}

void Plugin::cleanup() {
    if (m_isInitialized) {
        std::cout << "Cleaning up uevr Plugin..." << std::endl;
        
        if (m_framework) {
            m_framework->cleanup();
            m_framework.reset();
        }
        
        m_isInitialized = false;
        std::cout << "uevr Plugin cleanup completed" << std::endl;
    }
}

bool Plugin::isInitialized() const {
    return m_isInitialized;
}

Framework* Plugin::getFramework() {
    return m_framework.get();
}

} // namespace uevr
