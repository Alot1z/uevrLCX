# uevr Cross-Engine VR Adapters

## Overview

The uevr Cross-Engine VR Adapters provide VR support for non-Unreal Engine games by extending uevr's plugin architecture. This system enables VR adaptation for games built on RE Engine, REDengine 4, and MT Framework.

## Supported Games

| Game | Engine | Executable | Status |
|------|--------|------------|---------|
| Resident Evil 7 | RE Engine | `re7.exe` | ✅ Complete |
| Cyberpunk 2077 | REDengine 4 | `Cyberpunk2077.exe` | ✅ Complete |
| Monster Hunter: World | MT Framework | `MonsterHunterWorld.exe` | ✅ Complete |

## Architecture

### Core Components

```
src/adapters/
├── common/                          # Shared adapter functionality
│   ├── BaseCrossEngineAdapter.hpp   # Base class for all adapters
│   ├── BaseCrossEngineAdapter.cpp   # Base implementation
│   ├── CrossEngineAdapterFactory.hpp # Factory for creating adapters
│   └── CrossEngineAdapterFactory.cpp # Factory implementation
├── re-engine/                       # RE Engine adapter
│   ├── REEngineAdapter.hpp         # RE Engine adapter header
│   └── REEngineAdapter.cpp         # RE Engine adapter implementation
├── redengine4/                      # REDengine 4 adapter
│   ├── REDengine4Adapter.hpp       # REDengine 4 adapter header
│   └── REDengine4Adapter.cpp       # REDengine 4 adapter implementation
├── mt-framework/                    # MT Framework adapter
│   ├── MTFrameworkAdapter.hpp      # MT Framework adapter header
│   └── MTFrameworkAdapter.cpp      # MT Framework adapter implementation
├── tests/                          # Test suite
│   └── CrossEngineAdapterTests.cpp # Comprehensive test suite
├── main.cpp                        # Main entry point and demo
└── CMakeLists.txt                  # Build configuration
```

### Class Hierarchy

```
ICrossEngineAdapter (Interface)
    └── BaseCrossEngineAdapter (Abstract Base)
        ├── REEngineAdapter (RE Engine)
        ├── REDengine4Adapter (REDengine 4)
        └── MTFrameworkAdapter (MT Framework)

ICrossEngineAdapterFactory (Interface)
    └── CrossEngineAdapterFactory (Implementation)
```

## Features

### Universal VR Support
- **Stereo Rendering**: Automatic left/right eye rendering
- **Camera Tracking**: 6DoF head tracking integration
- **Performance Optimization**: Adaptive quality and frame timing
- **Error Handling**: Comprehensive error logging and recovery
- **Memory Management**: Safe memory scanning and validation

### Engine-Specific Features

#### RE Engine (Resident Evil 7)
- D3D11 rendering pipeline hooks
- Camera system integration
- Horror game optimization
- First-person VR experience

#### REDengine 4 (Cyberpunk 2077)
- D3D12 rendering pipeline hooks
- Constant buffer analysis
- Async compute support
- Cyberpunk aesthetic optimization

#### MT Framework (Monster Hunter: World)
- D3D11 rendering pipeline hooks
- Third-person to first-person conversion
- Animation system integration
- Combat mechanics adaptation

## Building

### Prerequisites
- CMake 3.16+
- Visual Studio 2019+ (Windows)
- DirectX 11/12 SDK
- Windows 10 SDK

### Build Commands

```bash
# Navigate to adapters directory
cd src/adapters

# Create build directory
mkdir build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release --parallel 8
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_RE_ENGINE_ADAPTER` | Build RE Engine adapter | ON |
| `BUILD_REDENGINE4_ADAPTER` | Build REDengine 4 adapter | ON |
| `BUILD_MT_FRAMEWORK_ADAPTER` | Build MT Framework adapter | ON |
| `BUILD_TESTS` | Build test suite | OFF |
| `BUILD_EXAMPLES` | Build examples | OFF |

## Usage

### Basic Usage

```cpp
#include "common/CrossEngineAdapterFactory.hpp"
#include "uevr/ICrossEngineAdapter.hpp"

using namespace uevr;

// Create factory
CrossEngineAdapterFactory factory;

// Create adapter for specific game
auto adapter = factory.createAdapter("re7.exe");
if (adapter) {
    // Initialize VR
    if (adapter->initialize()) {
        // Main loop
        while (adapter->isVREnabled()) {
            adapter->update();
            adapter->render();
        }
        
        // Cleanup
        adapter->cleanup();
    }
}
```

### Integration with uevr

The adapters automatically integrate with uevr's plugin system:

```cpp
class MyVRPlugin : public uevr::Plugin {
private:
    std::unique_ptr<ICrossEngineAdapter> m_adapter;
    
public:
    void on_initialize() override {
        // Auto-detect and create appropriate adapter
        CrossEngineAdapterFactory factory;
        m_adapter = factory.createAdapter(getCurrentGameExecutable());
        
        if (m_adapter) {
            m_adapter->initialize();
        }
    }
    
    void on_post_render_vr_framework_dx11(...) override {
        if (m_adapter && m_adapter->isVREnabled()) {
            m_adapter->render();
        }
    }
};
```

## Testing

### Run Test Suite

```bash
cd build
./tests/CrossEngineAdapterTests.exe
```

### Test Coverage

- ✅ Factory creation and game detection
- ✅ Adapter instantiation and properties
- ✅ Base adapter functionality
- ✅ Error handling and recovery
- ✅ Performance metrics tracking
- ✅ Memory management validation

## Performance

### Target Metrics

| Metric | Target | Notes |
|--------|--------|-------|
| Frame Time | < 16.67ms | 60 FPS target |
| VR Latency | < 20ms | Total motion-to-photon |
| Memory Usage | < 512MB | Additional VR overhead |
| CPU Overhead | < 5% | VR processing cost |

### Optimization Features

- **Adaptive Quality**: Automatic quality adjustment based on performance
- **Frame Skipping**: Intelligent frame dropping for stability
- **LOD Management**: Dynamic level-of-detail adjustment
- **Memory Pooling**: Efficient resource management

## Configuration

### Adapter Profiles

Each adapter maintains configuration profiles in JSON format:

```json
{
  "adapter_info": {
    "name": "RE Engine VR Adapter",
    "version": "1.0.0",
    "engine": "RE Engine"
  },
  "vr_configuration": {
    "ipd": 0.064,
    "near_plane": 0.1,
    "far_plane": 1000.0
  },
  "performance": {
    "target_fps": 60,
    "adaptive_quality": true,
    "max_quality_level": 1.0
  }
}
```

### Runtime Configuration

- **IPD Adjustment**: Real-time interpupillary distance tuning
- **Quality Presets**: Performance, Balanced, Quality modes
- **Camera Offsets**: Customizable camera positioning
- **Input Mapping**: Configurable controller bindings

## Troubleshooting

### Common Issues

#### Build Errors
- **Missing DirectX SDK**: Install latest DirectX 11/12 SDK
- **CMake Version**: Ensure CMake 3.16+ is installed
- **Compiler Version**: Use Visual Studio 2019+ or compatible compiler

#### Runtime Issues
- **Engine Detection Failed**: Verify executable name matches supported games
- **VR Initialization Failed**: Check VR runtime (OpenXR/SteamVR) installation
- **Performance Issues**: Adjust quality settings or enable adaptive quality

### Debug Information

Enable debug logging by setting environment variables:

```bash
# Enable detailed logging
set uevr_DEBUG=1
set uevr_LOG_LEVEL=DEBUG

# Enable performance profiling
set uevr_PROFILE=1
```

## Development

### Adding New Adapters

1. **Create Adapter Class**
   ```cpp
   class NewEngineAdapter : public BaseCrossEngineAdapter {
   protected:
       bool detectEngine() override;
       bool initializeEngineHooks() override;
       // ... implement required methods
   };
   ```

2. **Add to Factory**
   ```cpp
   // In CrossEngineAdapterFactory.cpp
   if (normalizedName.find("newgame") != std::string::npos) {
       return createNewEngineAdapter();
   }
   ```

3. **Update Build System**
   ```cmake
   # In CMakeLists.txt
   option(BUILD_NEW_ENGINE_ADAPTER "Build New Engine adapter" ON)
   ```

### Contributing

- Follow existing code style and patterns
- Add comprehensive tests for new features
- Update documentation for new adapters
- Ensure backward compatibility

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For issues, questions, or contributions:

- **Issues**: Create GitHub issue with detailed description
- **Discussions**: Use GitHub Discussions for questions
- **Contributions**: Submit pull requests with clear descriptions

## Roadmap

### Version 1.1
- [ ] Additional engine support (Frostbite, Source 2)
- [ ] Advanced VR features (foveated rendering, eye tracking)
- [ ] Performance profiling tools
- [ ] Configuration GUI

### Version 1.2
- [ ] Multi-GPU support
- [ ] Cloud VR integration
- [ ] Machine learning optimization
- [ ] Cross-platform support

---

**Note**: This system is designed for educational and research purposes. Ensure compliance with game terms of service and applicable laws when using these adapters.
