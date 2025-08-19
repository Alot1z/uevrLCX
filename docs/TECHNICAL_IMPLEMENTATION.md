# uevr Technical Implementation Guide

## 🔧 **TECHNISK IMPLEMENTERING**

Denne guide beskriver den tekniske implementering af uevr's nye struktur og hvordan udviklere kan arbejde med systemet.

## 📋 **SYSTEM ARKITEKTUR**

### **Core Framework (src/core/)**
```cpp
namespace uevr {
    class Framework {
    public:
        bool initialize();
        void run();
        void cleanup();
        bool isInitialized() const;
        
    private:
        std::unique_ptr<Plugin> m_plugin;
        bool m_isInitialized;
    };
}
```

### **Adapter System (src/adapters/)**
```cpp
namespace uevr {
namespace Adapters {
    class CrossEngineAdapter {
    public:
        virtual bool initialize() = 0;
        virtual void update() = 0;
        virtual void render() = 0;
        virtual void cleanup() = 0;
        
        virtual std::string getName() const = 0;
        virtual std::string getEngineType() const = 0;
        virtual std::string getVersion() const = 0;
    };
    
    class AdapterFactory {
    public:
        virtual std::shared_ptr<CrossEngineAdapter> createAdapter(const std::string& gameExecutable) = 0;
        virtual bool canCreateAdapter(const std::string& gameExecutable) const = 0;
        virtual std::string getSupportedEngineType() const = 0;
    };
}
}
```

### **Hook System (src/hooks/)**
```cpp
namespace uevr {
namespace Hooks {
    class DirectXHooks {
    public:
        bool installHooks();
        void removeHooks();
        
    private:
        bool installD3D11Hooks();
        bool installD3D12Hooks();
    };
    
    class HookManager {
    public:
        bool installAllHooks();
        void removeAllHooks();
        
    private:
        std::vector<std::unique_ptr<BaseHooks>> m_hooks;
    };
}
}
```

## 🏗️ **BUILD SYSTEM IMPLEMENTERING**

### **CMake Configuration Structure**

#### **Root CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.16)
project(uevr VERSION 1.0.0 LANGUAGES CXX)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build example implementations" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Dependencies
include(FetchContent)
find_package(MinHook QUIET)
find_package(spdlog QUIET)
find_package(nlohmann_json QUIET)

# Subdirectories
add_subdirectory(src/core)
add_subdirectory(src/adapters)
add_subdirectory(src/hooks)
add_subdirectory(src/mods)

# Main library
add_library(uevr SHARED src/main.cpp src/Plugin.cpp)
target_link_libraries(uevr uevr_core uevr_adapters_common)
```

#### **Component CMakeLists.txt Example**
```cmake
# src/adapters/mt-framework/CMakeLists.txt
set(MT_FRAMEWORK_SOURCES
    mt_framework_adapter.cpp
    mt_framework_factory.cpp
    mt_framework_hooks.cpp
    monster_hunter_world_adapter.cpp
)

add_library(mt_framework_adapter SHARED ${MT_FRAMEWORK_SOURCES})

target_include_directories(mt_framework_adapter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(mt_framework_adapter
    uevr_adapters_common
    uevr_core
    MinHook::MinHook
)
```

## 📁 **FILE ORGANIZATION RULES**

### **Header Files (include/uevr/)**
- **Public API**: Headers that external code should include
- **Internal**: Headers only used within the project
- **Naming**: Use PascalCase for class names, snake_case for files

### **Source Files (src/)**
- **Core**: Main framework functionality
- **Adapters**: Engine-specific implementations
- **Hooks**: API interception code
- **Mods**: VR modification code

### **Include Path Rules**
```cpp
// ✅ CORRECT - Use clean paths
#include "uevr/Framework.hpp"
#include "uevr/adapters/common/CrossEngineAdapter.hpp"
#include "uevr/hooks/DirectXHooks.hpp"

// ❌ WRONG - Don't use relative paths
#include "../../include/uevr/Framework.hpp"
#include "../common/CrossEngineAdapter.hpp"
```

## 🔄 **MIGRATION IMPLEMENTERING**

### **Step 1: File Movement Script**
```powershell
# migrate_files.ps1
$mappings = @{
    "adapters\MT-Framework" = "src\adapters\mt-framework"
    "adapters\RE-Engine" = "src\adapters\re-engine"
    "adapters\REDengine4" = "src\adapters\redengine4"
    "SOURCECODE\uevr\src" = "src\core"
}

foreach ($mapping in $mappings.GetEnumerator()) {
    $source = $mapping.Key
    $destination = $mapping.Value
    
    if (Test-Path $source) {
        Move-Item $source $destination -Force
        Write-Host "Moved $source to $destination"
    }
}
```

### **Step 2: Include Path Update Script**
```powershell
# update_includes.ps1
$includeMappings = @{
    '\.\.\/\.\.\/include\/uevr\/' = 'uevr/'
    '\.\.\/common\/' = 'uevr/adapters/common/'
    '\.\.\/\.\.\/SOURCECODE\/uevr\/src\/' = 'uevr/core/'
}

$files = Get-ChildItem -Path "src" -Filter "*.cpp" -Recurse
foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    foreach ($mapping in $includeMappings.GetEnumerator()) {
        $content = $content -replace $mapping.Key, $mapping.Value
    }
    Set-Content -Path $file.FullName -Value $content -NoNewline
}
```

## 🧪 **TESTING IMPLEMENTERING**

### **Unit Test Structure**
```cpp
// tests/unit/core_tests.cpp
#include <gtest/gtest.h>
#include "uevr/Framework.hpp"

namespace uevr {
namespace Tests {

class FrameworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        framework = std::make_unique<Framework>();
    }
    
    void TearDown() override {
        if (framework) {
            framework->cleanup();
        }
    }
    
    std::unique_ptr<Framework> framework;
};

TEST_F(FrameworkTest, InitializationTest) {
    EXPECT_TRUE(framework->initialize());
    EXPECT_TRUE(framework->isInitialized());
}

} // namespace Tests
} // namespace uevr
```

### **Integration Test Structure**
```cpp
// tests/integration/adapter_tests.cpp
#include <gtest/gtest.h>
#include "uevr/adapters/common/CrossEngineAdapter.hpp"

namespace uevr {
namespace Tests {

class AdapterIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test environment
    }
};

TEST_F(AdapterIntegrationTest, MTFrameworkAdapterCreation) {
    // Test adapter creation and basic functionality
}

} // namespace Tests
} // namespace uevr
```

## 🚀 **BUILD AND DEPLOYMENT**

### **Build Scripts**
```batch
@echo off
REM build.bat
if not exist "build" mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)

echo Build completed successfully!
```

### **PowerShell Build Script**
```powershell
# build.ps1
Write-Host "Building uevr Project..." -ForegroundColor Green

if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}
Set-Location "build"

try {
    cmake .. -G "Visual Studio 17 2022" -A x64
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
    
    cmake --build . --config Release
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
    
    Write-Host "Build completed successfully!" -ForegroundColor Green
} catch {
    Write-Host "Build failed: $_" -ForegroundColor Red
    exit 1
}
```

## 🔍 **DEBUGGING AND TROUBLESHOOTING**

### **Common Build Issues**

#### **Include Path Errors**
```cpp
// Error: Cannot open include file 'uevr/Framework.hpp'
// Solution: Check that include directory is properly set in CMake
target_include_directories(uevr PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/uevr
)
```

#### **Linker Errors**
```cpp
// Error: Unresolved external symbol
// Solution: Ensure all dependencies are properly linked
target_link_libraries(uevr
    uevr_core
    uevr_adapters_common
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)
```

#### **CMake Configuration Errors**
```cmake
# Error: Could not find package
# Solution: Use FetchContent for missing dependencies
include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)
```

### **Debug Configuration**
```cmake
# Enable debug information
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Enable verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)
```

## 📊 **PERFORMANCE OPTIMIZATION**

### **Build Optimization**
```cmake
# Enable parallel builds
set(CMAKE_BUILD_PARALLEL_LEVEL 8)

# Enable link time optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Enable optimizations for release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
```

### **Runtime Optimization**
```cpp
// Use const references where possible
void processData(const std::vector<int>& data);

// Avoid unnecessary copies
auto result = std::make_unique<Result>();

// Use move semantics
std::vector<int> createVector() {
    std::vector<int> vec;
    // ... populate vector
    return vec; // RVO will optimize this
}
```

## 🔮 **FUTURE ENHANCEMENTS**

### **Plugin System**
```cpp
// Future: Dynamic plugin loading
class PluginManager {
public:
    bool loadPlugin(const std::string& path);
    void unloadPlugin(const std::string& name);
    
private:
    std::unordered_map<std::string, std::unique_ptr<Plugin>> m_plugins;
};
```

### **Configuration System**
```cpp
// Future: JSON-based configuration
class ConfigManager {
public:
    bool loadConfig(const std::string& path);
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T{});
    
private:
    nlohmann::json m_config;
};
```

---

*Denne guide giver udviklere alle nødvendige oplysninger til at arbejde med uevr's nye struktur og implementere nye funktioner.*
