# UEVR Cross-Engine VR Adapters

This directory contains cross-engine VR adapters that extend UEVR's plugin architecture to support non-Unreal Engine games. These adapters enable VR functionality for games built on engines like RE Engine, REDengine 4, and MT Framework.

## Overview

The cross-engine adapters provide:
- **DirectX Hook Integration**: Intercept rendering calls to inject VR functionality
- **Camera Matrix Extraction**: Extract view and projection matrices for stereo rendering
- **Motion Controller Support**: Integrate VR controllers with game input systems
- **UI Projection**: Adapt game HUDs and menus for VR display
- **Performance Optimization**: VR-specific rendering optimizations

## Supported Engines

### 1. RE Engine (Capcom)
- **Target Game**: Resident Evil 7 (re7.exe)
- **Graphics API**: D3D11
- **Adapter**: `RE-Engine/engine_hooks.cpp`
- **Key Features**: Horror game VR optimization, audio spatialization

### 2. REDengine 4 (CD Projekt Red)
- **Target Game**: Cyberpunk 2077 (Cyberpunk2077.exe)
- **Graphics API**: D3D12
- **Adapter**: `REDengine4/redengine_adapter.cpp`
- **Key Features**: D3D12 pipeline integration, Cyberpunk UI adaptation

### 3. MT Framework (Capcom)
- **Target Game**: Monster Hunter: World (MonsterHunterWorld.exe)
- **Graphics API**: D3D11
- **Adapter**: `MT-Framework/mt_framework_bridge.cpp`
- **Key Features**: Third-person to first-person conversion, animation system integration

## Directory Structure

```
adapters/
├── RE-Engine/                    # RE Engine adapter
│   ├── engine_hooks.cpp         # Main adapter implementation
│   ├── RE_Engine_Adapter.hpp    # Header file
│   └── CMakeLists.txt           # Build configuration
├── REDengine4/                  # REDengine 4 adapter
│   ├── redengine_adapter.cpp    # Main adapter implementation
│   ├── redengine_adapter.hpp    # Header file
│   └── CMakeLists.txt           # Build configuration
├── MT-Framework/                # MT Framework adapter
│   ├── mt_framework_bridge.cpp  # Main adapter implementation
│   ├── mt_framework_adapter.hpp # Header file
│   └── CMakeLists.txt           # Build configuration
├── build_all.bat                # Build script for all adapters
└── README.md                    # This file
```

## Building the Adapters

### Prerequisites

- **Visual Studio 2019 or 2022** with C++ development tools
- **CMake 3.16 or later**
- **MinHook library** (automatically found by CMake)
- **DirectX SDK** (included with Visual Studio)

### Quick Build

1. **Run the build script**:
   ```batch
   adapters\build_all.bat
   ```

2. **Or build manually**:
   ```batch
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

### Individual Adapter Builds

```batch
# Build RE Engine adapter only
cmake --build . --target RE_Engine_Adapter --config Release

# Build REDengine 4 adapter only
cmake --build . --target REDengine4_Adapter --config Release

# Build MT Framework adapter only
cmake --build . --target MT_Framework_Adapter --config Release
```

## Architecture

### Plugin System Integration

Each adapter extends UEVR's plugin system by:
1. **Inheriting from `uevr::Plugin`**: Provides access to UEVR's callback system
2. **Implementing engine-specific hooks**: Using MinHook to intercept DirectX calls
3. **Integrating with UEVR's VR pipeline**: Leveraging existing stereo rendering and motion controller systems

### Hook Implementation

#### D3D11 Engines (RE Engine, MT Framework)
```cpp
// Hook IDXGISwapChain::Present (vtable index 8)
// Hook IDXGISwapChain::ResizeBuffers (vtable index 13)
```

#### D3D12 Engines (REDengine 4)
```cpp
// Hook IDXGISwapChain3::Present1 (vtable index 22)
// Hook ID3D12CommandQueue::ExecuteCommandLists (vtable index 10)
```

### Matrix Extraction Framework

Each adapter implements a framework for extracting camera matrices:
1. **Constant Buffer Analysis**: Identifies buffer slots containing view/projection matrices
2. **Matrix Layout Handling**: Supports different matrix formats (row/column major, handedness)
3. **VR Transformation**: Converts extracted matrices for VR stereo rendering

## Usage

### Installation

1. **Build the adapters** using the build script or manual CMake commands
2. **Copy the DLL files** to your UEVR profiles directory:
   ```
   profiles/
   ├── re7/
   │   └── adapters/
   │       └── RE_Engine_Adapter.dll
   ├── Cyberpunk2077/
   │   └── adapters/
   │       └── REDengine4_Adapter.dll
   └── MonsterHunterWorld/
       └── adapters/
           └── MT_Framework_Adapter.dll
   ```

### Configuration

Each adapter requires configuration in the game's profile:
- **adapter.json**: Engine integration parameters and matrix sources
- **config.txt**: VR settings and performance parameters
- **cameras.txt**: Camera tuning and comfort settings

## Development

### Adding New Engines

To add support for a new engine:

1. **Create adapter directory**:
   ```
   adapters/NewEngine/
   ├── new_engine_adapter.cpp
   ├── new_engine_adapter.hpp
   └── CMakeLists.txt
   ```

2. **Implement adapter class**:
   ```cpp
   class NewEngineAdapter : public uevr::Plugin {
       // Implement required methods
       virtual void on_initialize() override;
       virtual void on_device_reset() override;
       // ... other methods
   };
   ```

3. **Add build configuration** to `CMakeLists.txt`

4. **Update build script** to include new target

### Testing

1. **Compilation Testing**: Ensure all adapters compile without errors
2. **Hook Installation**: Verify successful DirectX function interception
3. **Matrix Extraction**: Test camera matrix extraction from target games
4. **VR Integration**: Validate stereo rendering and motion controller support

## Troubleshooting

### Common Issues

#### Build Failures
- **CMake not found**: Install CMake and add to PATH
- **Visual Studio not found**: Install Visual Studio with C++ tools
- **MinHook not found**: Ensure MinHook package is available

#### Runtime Issues
- **Hook installation failed**: Check MinHook initialization and function addresses
- **Matrix extraction failed**: Verify constant buffer analysis and buffer slot identification
- **Performance problems**: Profile rendering pipeline and optimize matrix operations

### Debug Techniques

1. **Logging**: Use spdlog for structured logging and debugging
2. **RenderDoc**: Capture and analyze rendering pipeline for matrix identification
3. **Memory Analysis**: Use debuggers for memory inspection and object layout verification

## Performance Considerations

### Optimization Strategies

1. **Matrix Operations**: Minimize matrix transformations and cache frequently used matrices
2. **Memory Access**: Efficient constant buffer tracking and minimal buffer updates
3. **Rendering**: Optimize stereo rendering pipeline and post-processing effects

### VR-Specific Optimizations

1. **Frame Timing**: Maintain 90+ FPS for comfortable VR experience
2. **Resolution Scaling**: Adaptive resolution based on performance
3. **LOD Management**: Optimize level of detail for VR rendering

## Future Enhancements

### Planned Features

1. **Advanced Stereo Rendering**: Multi-view rendering and variable rate shading
2. **Enhanced Motion Control**: Haptic feedback and advanced gesture recognition
3. **Performance Monitoring**: Real-time metrics and adaptive quality settings

### Engine Expansion

1. **Additional Engines**: Support for more game engines and frameworks
2. **Universal Adapter**: Generic adapter for unknown engine types
3. **Plugin Ecosystem**: Community-driven adapter development

## Contributing

### Development Guidelines

1. **Code Organization**: Keep engine-specific logic isolated and maintain clear interfaces
2. **Error Handling**: Implement comprehensive error checking and meaningful error messages
3. **Resource Management**: Properly manage DirectX resources and implement cleanup

### Testing Requirements

1. **Compilation**: All adapters must compile without warnings
2. **Hook Installation**: Verify successful DirectX function interception
3. **Matrix Extraction**: Validate camera matrix extraction accuracy
4. **VR Integration**: Test stereo rendering and motion controller functionality

## License

This project is based on UEVR's plugin system and follows the same licensing terms. See the main UEVR repository for license information.

## Support

For issues and questions:
1. **Check the troubleshooting section** above
2. **Review UEVR documentation** for plugin development guidance
3. **Submit issues** to the main UEVR repository
4. **Join the community** for development discussion and support

## Acknowledgments

- **UEVR Team**: For the excellent plugin architecture and VR framework
- **MinHook**: For the DirectX function interception capabilities
- **Community Contributors**: For testing, feedback, and development assistance
