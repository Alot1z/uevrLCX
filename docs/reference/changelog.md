# Changelog

This document provides a comprehensive history of changes, updates, and improvements made to UEVR across all versions.

## Version History Overview

### Release Schedule
- **Major Releases**: Every 3-6 months with significant new features
- **Minor Releases**: Monthly with improvements and bug fixes
- **Patch Releases**: Weekly with critical fixes and minor improvements
- **Beta Releases**: Continuous development builds for testing

### Version Numbering
- **Format**: `Major.Minor.Patch-Beta` (e.g., `2.1.3-beta.2`)
- **Major**: Breaking changes and major feature additions
- **Minor**: New features and improvements
- **Patch**: Bug fixes and minor improvements
- **Beta**: Pre-release testing versions

## Latest Release

### Version 2.1.3 (2024-01-15)

#### 🚀 New Features
- **Enhanced Engine Support**: Added support for Unreal Engine 5.3
- **Improved VR Performance**: New Alternate Frame Rendering (AFR) system
- **Advanced Input Mapping**: Customizable controller bindings and profiles
- **Performance Monitoring**: Real-time performance overlay and metrics

#### 🔧 Improvements
- **Better Compatibility**: Enhanced support for REDengine4 games
- **Optimized Rendering**: Improved stereo rendering pipeline
- **Enhanced Stability**: Better error handling and recovery
- **UI Improvements**: Modernized user interface and settings

#### 🐛 Bug Fixes
- Fixed crash when switching between VR and desktop modes
- Resolved input lag issues in certain games
- Fixed memory leaks in long gaming sessions
- Corrected stereo rendering artifacts

#### 📚 Documentation
- Complete API reference documentation
- Comprehensive troubleshooting guide
- Performance optimization guide
- Developer setup documentation

## Previous Releases

### Version 2.1.2 (2024-01-01)

#### 🚀 New Features
- **Lua Scripting Support**: Custom mod development with Lua
- **Advanced Hooking**: Improved function hooking system
- **Performance Profiles**: Pre-configured performance settings

#### 🔧 Improvements
- Enhanced MT-Framework compatibility
- Improved RE-Engine support
- Better error reporting and logging
- Optimized memory usage

#### 🐛 Bug Fixes
- Fixed compatibility issues with Windows 11 23H2
- Resolved VR input mapping problems
- Fixed performance degradation over time
- Corrected stereo rendering issues

### Version 2.1.1 (2023-12-15)

#### 🚀 New Features
- **Plugin System**: Extensible architecture for custom functionality
- **Advanced Configuration**: JSON-based configuration system
- **Performance Analytics**: Detailed performance metrics and analysis

#### 🔧 Improvements
- Enhanced OpenXR support
- Improved SteamVR integration
- Better game detection and compatibility
- Optimized rendering pipeline

#### 🐛 Bug Fixes
- Fixed crash on startup with certain antivirus software
- Resolved VR headset detection issues
- Fixed performance problems in specific games
- Corrected configuration loading errors

### Version 2.1.0 (2023-12-01)

#### 🚀 Major Features
- **Unreal Engine 5 Support**: Full compatibility with UE5.0-5.2
- **Advanced VR Features**: Room-scale tracking, haptic feedback
- **Performance Optimization**: Dynamic resolution, frame timing
- **Mod System**: Comprehensive modding framework

#### 🔧 Improvements
- Complete rewrite of rendering engine
- Enhanced input system with custom mappings
- Improved compatibility matrix
- Better error handling and recovery

#### 🐛 Bug Fixes
- Major stability improvements
- Fixed all known crash scenarios
- Resolved performance bottlenecks
- Corrected compatibility issues

### Version 2.0.0 (2023-11-01)

#### 🚀 Complete Rewrite
- **New Architecture**: Modular, extensible design
- **Cross-Platform Support**: Windows, Linux, macOS
- **Modern C++**: C++20 with modern design patterns
- **Performance Focus**: Optimized for VR performance

#### 🔧 Core Improvements
- Complete rewrite of hooking system
- New engine adapter framework
- Enhanced VR runtime support
- Improved configuration management

#### 🐛 Bug Fixes
- All previous issues resolved
- Complete stability overhaul
- Performance optimization
- Enhanced compatibility

## Legacy Versions

### Version 1.x Series (2023-01 to 2023-10)

#### Version 1.9.5 (2023-10-15)
- Final 1.x release
- Bug fixes and stability improvements
- Legacy compatibility maintenance

#### Version 1.9.0 (2023-09-01)
- Enhanced engine support
- Performance improvements
- Bug fixes and stability

#### Version 1.8.0 (2023-07-01)
- Major compatibility improvements
- New engine adapters
- Enhanced VR features

#### Version 1.7.0 (2023-05-01)
- Performance optimization
- Better error handling
- Improved compatibility

#### Version 1.6.0 (2023-03-01)
- Enhanced engine detection
- Improved stability
- Bug fixes

#### Version 1.5.0 (2023-01-01)
- Initial public release
- Basic VR functionality
- Core engine support

## Beta Releases

### Version 2.2.0-beta.1 (2024-01-20)

#### 🚀 Experimental Features
- **Vulkan Rendering**: Experimental Vulkan backend
- **Advanced Modding**: Enhanced mod development tools
- **Performance Profiling**: Detailed performance analysis tools

#### 🔧 Improvements
- Enhanced debugging capabilities
- Improved error reporting
- Better development tools

#### ⚠️ Known Issues
- Vulkan backend may be unstable
- Some mods may not work correctly
- Performance may vary

### Version 2.2.0-beta.2 (2024-01-25)

#### 🚀 New Features
- **Shader System**: Custom shader support
- **Advanced Hooking**: Improved function interception
- **Performance Monitoring**: Real-time metrics

#### 🔧 Improvements
- Better Vulkan stability
- Enhanced mod compatibility
- Improved performance

#### 🐛 Bug Fixes
- Fixed Vulkan rendering issues
- Resolved mod loading problems
- Corrected performance monitoring

## Development Builds

### Nightly Builds
- **Availability**: Daily development builds
- **Purpose**: Testing latest features and fixes
- **Stability**: May be unstable or incomplete
- **Download**: Available from GitHub Actions

### Feature Branches
- **Experimental Features**: New functionality in development
- **Performance Improvements**: Ongoing optimization work
- **Bug Fixes**: Critical issue resolution
- **Testing**: Quality assurance and validation

## Breaking Changes

### Version 2.0.0
- **Configuration Format**: Changed from INI to JSON
- **API Changes**: Complete API redesign
- **File Structure**: New directory organization
- **Dependencies**: Updated library requirements

### Version 2.1.0
- **Plugin System**: New plugin architecture
- **Configuration**: Enhanced configuration system
- **Performance**: New performance monitoring system

### Version 2.2.0
- **Vulkan Support**: New rendering backend
- **Mod System**: Enhanced modding capabilities
- **API**: Additional API endpoints

## Migration Guide

### From Version 1.x to 2.0.0
```bash
# 1. Backup configuration
cp uevr.ini uevr.ini.backup

# 2. Install new version
# Download and extract UEVR 2.0.0

# 3. Convert configuration
# Use configuration migration tool
uevr-config-migrate uevr.ini uevr.json

# 4. Test functionality
# Verify all features work correctly
```

### From Version 2.0.0 to 2.1.0
```bash
# 1. Update configuration
# Add new configuration options
# Enable new features as needed

# 2. Install plugins
# Download compatible plugins
# Configure plugin settings

# 3. Test new features
# Verify plugin functionality
# Check performance improvements
```

### From Version 2.1.0 to 2.2.0
```bash
# 1. Enable Vulkan (optional)
# Set rendering backend to Vulkan
# Test performance and stability

# 2. Update mods
# Ensure mods are compatible
# Test mod functionality

# 3. Configure new features
# Set up performance profiling
# Configure advanced options
```

## Deprecated Features

### Version 2.0.0
- **INI Configuration**: Replaced with JSON
- **Legacy Hooking**: Replaced with new system
- **Old API**: Replaced with new API
- **Legacy Mods**: Incompatible with new system

### Version 2.1.0
- **Basic Configuration**: Replaced with advanced system
- **Simple Modding**: Replaced with plugin system
- **Basic Performance**: Replaced with monitoring system

### Version 2.2.0
- **DirectX Only**: Added Vulkan support
- **Basic Modding**: Enhanced modding capabilities
- **Simple API**: Extended API functionality

## Known Issues

### Current Version (2.1.3)
- **Minor Issue**: Occasional input lag in specific games
- **Workaround**: Adjust input sensitivity settings
- **Status**: Under investigation

- **Minor Issue**: Memory usage increase over time
- **Workaround**: Restart UEVR periodically
- **Status**: Optimization in progress

### Previous Versions
- **Version 2.1.2**: Fixed crash on startup
- **Version 2.1.1**: Resolved VR detection issues
- **Version 2.1.0**: Fixed performance problems

## Future Releases

### Version 2.2.0 (Planned: 2024-02-01)
- **Vulkan Rendering**: Stable Vulkan backend
- **Advanced Modding**: Enhanced mod development
- **Performance Tools**: Comprehensive profiling

### Version 2.3.0 (Planned: 2024-04-01)
- **Cross-Platform**: Linux and macOS support
- **Cloud Features**: Configuration sync and sharing
- **Advanced VR**: Enhanced VR capabilities

### Version 3.0.0 (Planned: 2024-07-01)
- **Major Architecture**: Complete system redesign
- **New Features**: Revolutionary VR functionality
- **Performance**: Next-generation optimization

## Release Notes

### How to Read Release Notes
- **🚀 New Features**: New functionality added
- **🔧 Improvements**: Enhanced existing features
- **🐛 Bug Fixes**: Issues resolved
- **⚠️ Known Issues**: Current limitations
- **📚 Documentation**: Documentation updates
- **🔒 Security**: Security-related changes

### Release Categories
- **Stable Release**: Production-ready software
- **Beta Release**: Feature-complete testing
- **Alpha Release**: Early feature testing
- **Development Build**: Latest development work

## Support and Updates

### Update Channels
- **Stable**: Production releases (recommended)
- **Beta**: Pre-release testing
- **Development**: Latest development builds
- **Custom**: Specific version requirements

### Update Methods
- **Automatic**: Built-in update checker
- **Manual**: Download from GitHub
- **Package Manager**: System package managers
- **Source**: Build from source code

### Support Policy
- **Current Version**: Full support
- **Previous Version**: Limited support
- **Legacy Versions**: No support
- **Beta Versions**: Community support

## Resources

- [UEVR Installation Guide](../installation.md)
- [Performance Optimization](../performance/overview.md)
- [Development Setup](../development/setup.md)
- [API Reference](../development/api-reference.md)
- [Troubleshooting Guide](troubleshooting.md)

---

*For the latest release information, check the GitHub repository or join the community Discord.*
