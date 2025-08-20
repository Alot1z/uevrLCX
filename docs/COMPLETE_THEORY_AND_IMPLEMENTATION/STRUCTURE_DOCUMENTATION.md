# uevr Project Structure Documentation

## 📋 **PROJEKT OVERBLIK**

uevr (Unreal Engine VR Modding Framework) er et avanceret cross-engine VR modding framework der understøtter multiple game engines og giver en enkelt platform til VR integration.

## 🏗️ **STRUKTUR TRANSFORMATION**

### **Fra Gammel til Ny Struktur**

#### **Gammel Struktur (Uorganiseret)**
```
uevr/
├── adapters/                    # Scattered adapter implementations
│   ├── MT-Framework/           # Monster Hunter World
│   ├── RE-Engine/              # Resident Evil 7
│   └── REDengine4/             # Cyberpunk 2077
├── SOURCECODE/uevr/src/        # Mixed source files
│   ├── Framework.cpp            # Core functionality
│   ├── hooks/                  # API hooking
│   ├── mods/                   # VR modifications
│   └── utility/                # Utilities
└── include/                     # Public headers
    └── uevr/                   # API definitions
```

#### **Ny Struktur (Organiseret)**
```
uevr/
├── src/                        # All source code
│   ├── core/                   # Core framework
│   ├── adapters/               # Engine adapters
│   ├── hooks/                  # API hooking system
│   ├── mods/                   # VR modification system
│   └── utils/                  # Utility functions
├── include/                     # Public API headers
│   └── uevr/                   # uevr API
├── tests/                       # Test suite
├── examples/                    # Example implementations
├── docs/                        # Documentation
└── build/                       # Build outputs
```

## 📁 **DETALJERET MAPPE STRUKTUR**

### **src/core/** - Core Framework
**Ansvar**: Hovedfunktionalitet, framework initialization, exception handling

**Filer**:
- `Framework.cpp` - Main framework implementation
- `Framework.hpp` - Framework interface
- `ExceptionHandler.cpp` - Exception handling system
- `Plugin.cpp` - Plugin management system
- `main.cpp` - Application entry point
- `utility/` - Core utility functions

**Dependencies**:
- spdlog (logging)
- nlohmann_json (configuration)
- Windows API (system integration)

### **src/adapters/** - Engine Adapters
**Ansvar**: Game engine specific VR integration

**Undermapper**:
- `common/` - Shared adapter code
  - `CrossEngineAdapter.cpp` - Base adapter interface
  - `AdapterFactory.cpp` - Factory pattern implementation
  - `GameProfile.cpp` - Game configuration profiles

- `mt-framework/` - MT Framework (Monster Hunter World)
  - `mt_framework_adapter.cpp` - Main adapter implementation
  - `mt_framework_factory.cpp` - Factory for MT Framework
  - `mt_framework_hooks.cpp` - Engine-specific hooks
  - `monster_hunter_world_adapter.cpp` - Game-specific logic

- `re-engine/` - RE Engine (Resident Evil 7)
  - `re7_adapter.cpp` - Main adapter implementation
  - `RE_Engine_Hooks.cpp` - Engine-specific hooks
  - `engine_hooks.cpp` - Generic engine hooks
  - `re_engine_factory.cpp` - Factory implementation

- `redengine4/` - REDengine 4 (Cyberpunk 2077)
  - `cyberpunk2077_adapter.cpp` - Game-specific adapter
  - `redengine4_hooks.cpp` - Engine-specific hooks
  - `redengine4_factory.cpp` - Factory implementation
  - `cyberpunk2077_profile.cpp` - Game configuration

**Dependencies**:
- uevr_core
- MinHook (API hooking)
- DirectX libraries (graphics integration)

### **src/hooks/** - API Hooking System
**Ansvar**: Intercepting and modifying graphics API calls

**Filer**:
- `DirectXHooks.cpp` - DirectX 11/12 interception
- `OpenGLHooks.cpp` - OpenGL interception
- `VulkanHooks.cpp` - Vulkan interception
- `EngineHooks.cpp` - Engine-specific hooking
- `HookManager.cpp` - Centralized hook management

**Dependencies**:
- uevr_core
- MinHook (hook library)
- Graphics API headers

### **src/mods/** - VR Modification System
**Ansvar**: VR-specific modifications and enhancements

**Filer**:
- `VRModManager.cpp` - VR system management
- `CameraMod.cpp` - Camera modifications
- `ControllerMod.cpp` - Controller support
- `DisplayMod.cpp` - Display handling
- `AudioMod.cpp` - Audio modifications
- `PerformanceMod.cpp` - Performance optimization

**Dependencies**:
- uevr_core
- uevr_hooks
- OpenVR/OpenXR libraries

## 🔧 **BUILD SYSTEM STRUKTUR**

### **Hoved CMakeLists.txt**
**Placering**: Root directory
**Ansvar**: 
- Projekt konfiguration
- Dependency management
- Subdirectory inclusion
- Build options

**Hovedkomponenter**:
- C++ standard (C++17)
- Build options (tests, examples, docs)
- Output directory configuration
- Dependency setup (MinHook, spdlog, nlohmann_json)

### **Komponent CMakeLists.txt**

#### **src/core/CMakeLists.txt**
- Core library definition
- Source file compilation
- Include path configuration
- Dependency linking

#### **src/adapters/CMakeLists.txt**
- Common adapter library
- Subdirectory inclusion
- Shared dependency management

#### **src/adapters/mt-framework/CMakeLists.txt**
- MT Framework adapter library
- Source file compilation
- Include path configuration
- Dependency linking

#### **src/adapters/re-engine/CMakeLists.txt**
- RE Engine adapter library
- DirectX integration
- Source file compilation

#### **src/adapters/redengine4/CMakeLists.txt**
- REDengine 4 adapter library
- Cyberpunk 2077 integration
- Source file compilation

#### **src/hooks/CMakeLists.txt**
- Hooks library definition
- API hooking compilation
- MinHook integration

#### **src/mods/CMakeLists.txt**
- Mods library definition
- VR modification compilation
- Hook system integration

#### **tests/CMakeLists.txt**
- Test suite configuration
- Google Test integration
- Test execution setup

#### **examples/CMakeLists.txt**
- Example compilation
- Basic plugin examples
- Custom adapter examples

## 📝 **INCLUDE PATH STRUKTUR**

### **Gamle Include Paths (Uorganiserede)**
```cpp
// Relative paths causing confusion
#include "../../include/uevr/CrossEngineAdapterRegistry.hpp"
#include "../common/matrix_extraction.hpp"
#include "../../SOURCECODE/uevr/src/Framework.hpp"
#include "../../include/uevr/Plugin.hpp"
```

### **Nye Include Paths (Organiserede)**
```cpp
// Clean, organized paths
#include "uevr/CrossEngineAdapterRegistry.hpp"
#include "uevr/adapters/common/matrix_extraction.hpp"
#include "uevr/core/Framework.hpp"
#include "uevr/Plugin.hpp"
```

### **Include Path Mapping**
| Gammel Path | Ny Path |
|-------------|---------|
| `../../include/uevr/` | `uevr/` |
| `../common/` | `uevr/adapters/common/` |
| `../../SOURCECODE/uevr/src/` | `uevr/core/` |
| `../MT-Framework/` | `uevr/adapters/mt-framework/` |
| `../RE-Engine/` | `uevr/adapters/re-engine/` |
| `../REDengine4/` | `uevr/adapters/redengine4/` |

## 🚀 **MIGRATION PROCESS**

### **Fase 1: Struktur Oprettelse ✅**
- [x] Ny mappestruktur oprettet
- [x] CMake konfiguration filer oprettet
- [x] Placeholder filer oprettet
- [x] Build scripts oprettet

### **Fase 2: Fil Migration 🔄**
- [ ] Flyt alle eksisterende filer til nye mapper
- [ ] Opdater alle include paths
- [ ] Ret namespace strukturer
- [ ] Valider fil integritet

### **Fase 3: Build System 🔄**
- [ ] Test CMake konfiguration
- [ ] Fix dependency issues
- [ ] Valider build process
- [ ] Test alle komponenter

### **Fase 4: Testing & Validering 🔄**
- [ ] Kompiler alle komponenter
- [ ] Kør unit tests
- [ ] Valider funktionalitet
- [ ] Performance testing

## 📊 **DEPENDENCY GRAPH**

```
uevr (main library)
├── uevr_core
│   ├── spdlog
│   └── nlohmann_json
├── uevr_adapters_common
│   ├── uevr_core
│   ├── spdlog
│   └── nlohmann_json
├── mt_framework_adapter
│   ├── uevr_adapters_common
│   ├── uevr_core
│   ├── MinHook
│   └── DirectX libraries
├── re_engine_adapter
│   ├── uevr_adapters_common
│   ├── uevr_core
│   ├── MinHook
│   └── DirectX libraries
├── redengine4_adapter
│   ├── uevr_adapters_common
│   ├── uevr_core
│   ├── MinHook
│   └── DirectX libraries
├── uevr_hooks
│   ├── uevr_core
│   └── MinHook
└── uevr_mods
    ├── uevr_core
    ├── uevr_hooks
    └── VR runtime libraries
```

## 🎯 **FORDELE VED NY STRUKTUR**

### **Organisering**
- **Klart ansvar**: Hver mappe har et specifikt formål
- **Logisk gruppering**: Relaterede filer er samlet
- **Skalerbarhed**: Nem at tilføje nye komponenter

### **Build System**
- **Modulær**: Hver komponent kan bygges uafhængigt
- **Konfigurerbar**: Nem at enable/disable komponenter
- **Dependency management**: Klare afhængigheder

### **Development**
- **Include paths**: Konsistente og intuitive
- **Testing**: Organiseret test struktur
- **Examples**: Klare implementeringseksempler

### **Maintenance**
- **Fejlfinding**: Nemmere at lokalisere problemer
- **Opdateringer**: Isolerede komponenter
- **Dokumentation**: Struktureret og organiseret

## 🔮 **FREMTIDIGE FORBEDRINGER**

### **Kortsigtet (1-2 måneder)**
- [ ] Komplet fil migration
- [ ] Build system validering
- [ ] Test suite implementering
- [ ] Dokumentation opdatering

### **Mellemlangt (3-6 måneder)**
- [ ] CI/CD pipeline
- [ ] Automated testing
- [ ] Performance benchmarking
- [ ] Code quality tools

### **Langtid (6+ måneder)**
- [ ] Plugin ecosystem
- [ ] Community contributions
- [ ] Advanced VR features
- [ ] Cross-platform support

---

*Dette dokument beskriver den komplette struktur transformation for uevr projektet. Alle ændringer er designet til at forbedre organisering, vedligeholdelse og skalerbarhed.*
