# UEVR Game Compatibility Matrix

This document provides a comprehensive overview of game compatibility with UEVR, including testing status, known issues, and optimal configuration settings.

## Compatibility Status Legend

- 🟢 **Fully Compatible** - Game works perfectly with all VR features
- 🟡 **Partially Compatible** - Game works with some limitations or minor issues
- 🔴 **Incompatible** - Game has significant issues or doesn't work
- ⚪ **Untested** - Game hasn't been tested yet
- 🔧 **Requires Configuration** - Game needs specific settings to work properly

## Supported Game Engines

### Unreal Engine 4
- **UE4.18 - UE4.27**: Full support with optimized VR rendering
- **UE4.16 - UE4.17**: Limited support, may require specific configurations

### Unreal Engine 5
- **UE5.0 - UE5.3**: Full support with advanced VR features
- **UE5.4+**: Experimental support, may have compatibility issues

## Game Compatibility Matrix

### Action & Adventure Games

| Game | Engine | Status | VR Features | Notes |
|------|--------|--------|-------------|-------|
| Cyberpunk 2077 | REDengine4 | 🟢 Fully Compatible | Full VR support | Optimized for VR, excellent performance |
| Monster Hunter World | MT-Framework | 🟡 Partially Compatible | Basic VR support | Some UI scaling issues, requires configuration |
| Resident Evil 7 | RE-Engine | 🟢 Fully Compatible | Full VR support | Excellent VR implementation |
| Resident Evil 8 | RE-Engine | 🟢 Fully Compatible | Full VR support | Optimized VR rendering pipeline |

### RPG Games

| Game | Engine | Status | VR Features | Notes |
|------|--------|--------|-------------|-------|
| Abiotic Factor | Unreal Engine | 🟡 Partially Compatible | Basic VR support | Early access, ongoing compatibility work |
| The Outer Worlds | Unreal Engine | 🟢 Fully Compatible | Full VR support | Excellent VR adaptation |

### First-Person Games

| Game | Engine | Status | VR Features | Notes |
|------|--------|--------|-------------|-------|
| Metro Exodus | Unreal Engine | 🟢 Fully Compatible | Full VR support | Optimized for VR performance |
| Control | Northlight Engine | 🔴 Incompatible | Not supported | Engine not compatible with UEVR |

### Strategy & Simulation Games

| Game | Engine | Status | VR Features | Notes |
|------|--------|--------|-------------|-------|
| XCOM 2 | Unreal Engine | 🟡 Partially Compatible | Limited VR support | Turn-based gameplay limitations |

## Engine-Specific Compatibility

### REDengine4 Games
```cpp
// REDengine4 adapter configuration
class REDengine4Adapter : public IEngineAdapter {
public:
    bool Initialize() override {
        // REDengine4 specific initialization
        if (!InitializeREDengine4Hooks()) {
            return false;
        }
        
        // Configure VR rendering pipeline
        if (!SetupVRStereoRendering()) {
            return false;
        }
        
        return true;
    }
    
private:
    bool InitializeREDengine4Hooks();
    bool SetupVRStereoRendering();
};
```

**Compatible Games:**
- Cyberpunk 2077
- The Witcher 3 (with mods)

### MT-Framework Games
```cpp
// MT-Framework adapter configuration
class MTFrameworkAdapter : public IEngineAdapter {
public:
    bool Initialize() override {
        // MT-Framework specific initialization
        if (!InitializeMTFrameworkHooks()) {
            return false;
        }
        
        // Setup VR input handling
        if (!SetupVRInputSystem()) {
            return false;
        }
        
        return true;
    }
    
private:
    bool InitializeMTFrameworkHooks();
    bool SetupVRInputSystem();
};
```

**Compatible Games:**
- Monster Hunter World
- Monster Hunter Rise
- Devil May Cry 5

### RE-Engine Games
```cpp
// RE-Engine adapter configuration
class REEngineAdapter : public IEngineAdapter {
public:
    bool Initialize() override {
        // RE-Engine specific initialization
        if (!InitializeREEngineHooks()) {
            return false;
        }
        
        // Configure VR rendering
        if (!SetupVRRendering()) {
            return false;
        }
        
        return true;
    }
    
private:
    bool InitializeREEngineHooks();
    bool SetupVRRendering();
};
```

**Compatible Games:**
- Resident Evil 7
- Resident Evil 8
- Devil May Cry 5

## Performance Compatibility

### High-Performance Games
Games that run smoothly in VR with minimal performance impact:

```cpp
struct PerformanceProfile {
    std::string gameName;
    float targetFPS;
    bool enableAFR;
    bool enableDynamicResolution;
    int renderScale;
};

std::vector<PerformanceProfile> highPerformanceGames = {
    {"Cyberpunk 2077", 90.0f, true, true, 100},
    {"Resident Evil 7", 90.0f, true, false, 90},
    {"Resident Evil 8", 90.0f, true, true, 85}
};
```

### Medium-Performance Games
Games that require some performance optimization:

```cpp
std::vector<PerformanceProfile> mediumPerformanceGames = {
    {"Monster Hunter World", 72.0f, false, true, 80},
    {"The Outer Worlds", 80.0f, true, true, 85}
};
```

### Low-Performance Games
Games that may struggle in VR and require significant optimization:

```cpp
std::vector<PerformanceProfile> lowPerformanceGames = {
    {"XCOM 2", 60.0f, false, false, 70},
    {"Abiotic Factor", 72.0f, false, true, 75}
};
```

## Configuration Requirements

### Essential VR Settings
```ini
[VR]
enableVR=true
renderScale=85
targetFPS=90
enableAFR=true
enableDynamicResolution=true
```

### Game-Specific Configurations

#### Cyberpunk 2077
```ini
[Cyberpunk2077]
vrMode=full
performanceMode=high
enableRayTracing=false
enableDLSS=true
vrRenderScale=100
```

#### Monster Hunter World
```ini
[MonsterHunterWorld]
vrMode=basic
performanceMode=medium
enableAFR=false
vrRenderScale=80
uiScale=1.2
```

#### Resident Evil 7/8
```ini
[ResidentEvil]
vrMode=full
performanceMode=high
enableAFR=true
vrRenderScale=90
enableHDR=false
```

## Testing and Validation

### Testing Methodology
```cpp
class CompatibilityTester {
public:
    struct TestResult {
        std::string gameName;
        bool vrInitialization;
        bool renderingWorks;
        bool inputWorks;
        bool performanceAcceptable;
        std::string notes;
    };
    
    TestResult TestGame(const std::string& gameName);
    
private:
    bool TestVRInitialization();
    bool TestRendering();
    bool TestInput();
    bool TestPerformance();
};
```

### Validation Criteria
1. **VR Initialization**: Game successfully enters VR mode
2. **Rendering**: Stereo rendering works without artifacts
3. **Input**: VR controllers are recognized and functional
4. **Performance**: Maintains target frame rate
5. **Stability**: No crashes or major glitches

## Known Issues and Workarounds

### Common Issues

#### UI Scaling Problems
```cpp
// UI scaling fix for certain games
class UIScaler {
public:
    void FixUIScaling(float scaleFactor) {
        // Apply UI scaling adjustments
        for (auto& uiElement : uiElements) {
            uiElement->SetScale(scaleFactor);
        }
    }
};
```

#### Performance Issues
```cpp
// Performance optimization for struggling games
class PerformanceOptimizer {
public:
    void OptimizeForGame(const std::string& gameName) {
        if (gameName == "Monster Hunter World") {
            // Disable AFR for better stability
            DisableAFR();
            // Reduce render scale
            SetRenderScale(0.8f);
        }
    }
};
```

### Game-Specific Issues

#### Cyberpunk 2077
- **Issue**: Occasional frame drops in dense areas
- **Workaround**: Enable dynamic resolution scaling
- **Status**: Resolved in latest versions

#### Monster Hunter World
- **Issue**: UI elements too small in VR
- **Workaround**: Use UI scaling configuration
- **Status**: Ongoing improvement

#### Resident Evil 7/8
- **Issue**: HDR conflicts with VR rendering
- **Workaround**: Disable HDR in game settings
- **Status**: Resolved

## Future Compatibility

### Planned Support
- **Unreal Engine 5.4+**: Full compatibility planned
- **New REDengine4 games**: Automatic detection and support
- **Additional MT-Framework games**: Extended compatibility

### Community Testing
```cpp
// Community testing framework
class CommunityTester {
public:
    void SubmitTestResult(const TestResult& result);
    std::vector<TestResult> GetCommunityResults();
    float GetCompatibilityScore(const std::string& gameName);
};
```

## Contributing to Compatibility

### Testing New Games
1. Install UEVR and the target game
2. Run compatibility tests
3. Document any issues or workarounds
4. Submit test results to the community

### Reporting Issues
```cpp
struct IssueReport {
    std::string gameName;
    std::string gameVersion;
    std::string uevrVersion;
    std::string description;
    std::string reproductionSteps;
    std::string systemInfo;
    std::string logs;
};
```

## Resources

- [UEVR Installation Guide](../installation.md)
- [Performance Optimization Guide](../performance/overview.md)
- [Troubleshooting Guide](../troubleshooting.md)
- [Community Discord](https://discord.gg/uevr)
- [GitHub Issues](https://github.com/your-repo/uevr/issues)

---

*Last updated: 2024*
*For the most current compatibility information, check the community Discord or GitHub repository.*
