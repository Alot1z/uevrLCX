/*
 * uevr Custom Adapter Example
 * 
 * Demonstrates how to create a custom adapter for uevr
 * Copyright (c) 2024 uevr Project
 */

#include <iostream>
#include <string>

class CustomAdapter {
public:
    CustomAdapter() {
        std::cout << "Custom Adapter initialized" << std::endl;
    }
    
    ~CustomAdapter() {
        std::cout << "Custom Adapter destroyed" << std::endl;
    }
    
    void initialize() {
        std::cout << "Custom Adapter: Initializing..." << std::endl;
    }
    
    void process_frame() {
        std::cout << "Custom Adapter: Processing frame..." << std::endl;
    }
    
    void shutdown() {
        std::cout << "Custom Adapter: Shutting down..." << std::endl;
    }
};

int main() {
    std::cout << "uevr Custom Adapter Example" << std::endl;
    std::cout << "============================" << std::endl;
    
    CustomAdapter adapter;
    adapter.initialize();
    
    for (int i = 0; i < 5; i++) {
        adapter.process_frame();
    }
    
    adapter.shutdown();
    
    std::cout << "Example completed successfully!" << std::endl;
    return 0;
}
