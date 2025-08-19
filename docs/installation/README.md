# 🚀 UEVR Installation Guide

**Complete installation guide for the UEVR Cross-Engine VR System**

This guide will walk you through installing UEVR on your Windows system, from basic requirements to advanced configuration.

## 📋 **System Requirements**

### **Minimum Requirements**
- **OS**: Windows 10 64-bit (version 1903 or later)
- **CPU**: Intel Core i5-4590 / AMD FX 8350 or equivalent
- **RAM**: 8 GB RAM
- **GPU**: DirectX 11 compatible graphics card with 4GB VRAM
- **Storage**: 2 GB available space
- **VR Headset**: Oculus Rift, HTC Vive, Valve Index, or compatible

### **Recommended Requirements**
- **OS**: Windows 11 64-bit
- **CPU**: Intel Core i7-8700K / AMD Ryzen 7 3700X or equivalent
- **RAM**: 16 GB RAM
- **GPU**: DirectX 12 compatible graphics card with 8GB+ VRAM
- **Storage**: 5 GB available space (SSD recommended)
- **VR Headset**: Latest generation VR headset

### **VR Runtime Requirements**
- **OpenVR**: SteamVR 2.0 or later
- **OpenXR**: Windows Mixed Reality or compatible runtime
- **SteamVR**: Steam client with VR support

## 🔧 **Prerequisites Installation**

### **1. Visual C++ Redistributables**
```bash
# Download and install the latest Visual C++ Redistributables
# x64: https://aka.ms/vs/17/release/vc_redist.x64.exe
# x86: https://aka.ms/vs/17/release/vc_redist.x86.exe
```

### **2. .NET Framework 4.8**
```bash
# Download and install .NET Framework 4.8
# https://dotnet.microsoft.com/download/dotnet-framework/net48
```

### **3. DirectX End-User Runtime**
```bash
# Download and install DirectX End-User Runtime
# https://www.microsoft.com/en-us/download/details.aspx?id=35
```

### **4. VR Runtime Setup**
```bash
# SteamVR (Recommended)
1. Install Steam
2. Enable VR support in Steam settings
3. Install SteamVR from Steam library

# Oculus (Alternative)
1. Install Oculus software
2. Set up Oculus headset
3. Enable developer mode if needed

# Windows Mixed Reality (Alternative)
1. Install Windows Mixed Reality
2. Set up WMR headset
3. Configure room boundaries
```

## 📥 **Download UEVR**

### **Option 1: Pre-built Release (Recommended)**
1. Go to [UEVR Releases](https://github.com/uevr/uevr/releases)
2. Download the latest release for Windows
3. Extract to your preferred installation directory

### **Option 2: Build from Source**
```bash
# Clone the repository
git clone https://github.com/uevr/uevr.git
cd uevr

# Install build dependencies
vcpkg install spdlog nlohmann-json minhook

# Build with CMake
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## 🏗️ **Installation Steps**

### **Step 1: Extract Files**
```bash
# Extract UEVR to a permanent location
# Recommended: C:\Program Files\UEVR\
# or: C:\Users\[Username]\Documents\UEVR\
```

### **Step 2: Run Initial Setup**
```bash
# Navigate to UEVR directory
cd "C:\Program Files\UEVR"

# Run initial setup (as Administrator)
uevr_setup.exe --install
```

### **Step 3: Configure VR Runtime**
```bash
# Edit config/vr_runtime.json
{
  "primary_runtime": "OpenVR",
  "fallback_runtimes": ["OpenXR", "SteamVR"],
  "auto_detect": true,
  "preferred_headset": "auto"
}
```

### **Step 4: Install Engine Adapters**
```bash
# The system will automatically download and install:
# - RE Engine adapter (Resident Evil 7/8)
# - REDengine 4 adapter (Cyberpunk 2077)
# - MT Framework adapter (Monster Hunter World)
# - Additional engine adapters
```

## ⚙️ **Configuration**

### **Basic Configuration**
```bash
# Edit config/system.json
{
  "auto_detect_games": true,
  "auto_load_adapters": true,
  "performance_mode": "balanced",
  "vr_quality": "high",
  "motion_controllers": true,
  "room_scale": true
}
```

### **Game-Specific Configuration**
```bash
# Edit config/games/[game_name].json
{
  "engine": "RE_ENGINE",
  "adapter": "re_engine_adapter",
  "vr_settings": {
    "fov_multiplier": 1.2,
    "comfort_mode": true,
    "motion_blur_reduction": true
  }
}
```

### **Performance Configuration**
```bash
# Edit config/performance.json
{
  "async_reprojection": true,
  "motion_smoothing": true,
  "adaptive_quality": true,
  "target_fps": 90,
  "quality_presets": {
    "low": "performance",
    "medium": "balanced",
    "high": "quality",
    "ultra": "maximum"
  }
}
```

## 🎮 **Game Setup**

### **Supported Games Installation**
```bash
# Resident Evil 7 BIOHAZARD
1. Install from Steam/Epic/Retail
2. Launch game once to generate files
3. UEVR will auto-detect and configure

# Cyberpunk 2077
1. Install from Steam/GOG/Epic
2. Update to latest version
3. UEVR will auto-detect REDengine 4

# Monster Hunter World
1. Install from Steam
2. Launch game once
3. UEVR will auto-detect MT Framework
```

### **Game Detection**
```bash
# UEVR automatically detects:
# - Running game processes
# - Game engine type
# - Compatible VR adapter
# - Optimal VR settings
```

## 🔍 **Verification & Testing**

### **System Verification**
```bash
# Run system verification
uevr_verify.exe --full

# Expected output:
# ✅ VR Runtime: OpenVR detected
# ✅ Engine Adapters: 12/12 loaded
# ✅ Hook System: Ready
# ✅ VR System: Initialized
```

### **VR Headset Test**
```bash
# Test VR headset connection
uevr_test.exe --vr-headset

# Expected output:
# ✅ Headset: [Headset Name] detected
# ✅ Controllers: [Count] controllers found
# ✅ Tracking: Room scale available
```

### **Game Compatibility Test**
```bash
# Test specific game compatibility
uevr_test.exe --game "Resident Evil 7"

# Expected output:
# ✅ Engine: RE Engine detected
# ✅ Adapter: re_engine_adapter loaded
# ✅ VR Mode: Ready for activation
```

## 🚨 **Troubleshooting**

### **Common Issues**

#### **VR Headset Not Detected**
```bash
# Solution 1: Check VR runtime
uevr_diagnose.exe --vr-runtime

# Solution 2: Restart VR services
uevr_service.exe --restart-vr

# Solution 3: Reinstall VR runtime
# SteamVR: Verify files in Steam
# Oculus: Repair installation
```

#### **Game Not Detected**
```bash
# Solution 1: Check game process
uevr_diagnose.exe --game-processes

# Solution 2: Manual engine detection
uevr_detect.exe --manual "game.exe"

# Solution 3: Update engine signatures
uevr_update.exe --signatures
```

#### **Performance Issues**
```bash
# Solution 1: Performance profiling
uevr_profile.exe --performance

# Solution 2: Quality adjustment
uevr_config.exe --quality "balanced"

# Solution 3: Driver update
# Update GPU drivers to latest version
```

### **Advanced Troubleshooting**
```bash
# Full system diagnosis
uevr_diagnose.exe --full --verbose

# Log analysis
uevr_logs.exe --analyze --export

# System restore
uevr_restore.exe --last-known-good
```

## 🔄 **Updates & Maintenance**

### **Automatic Updates**
```bash
# Enable automatic updates
uevr_config.exe --auto-update true

# Check for updates
uevr_update.exe --check

# Install updates
uevr_update.exe --install
```

### **Manual Updates**
```bash
# Update specific components
uevr_update.exe --component engine-signatures
uevr_update.exe --component adapters
uevr_update.exe --component vr-system

# Full system update
uevr_update.exe --all
```

### **Backup & Restore**
```bash
# Create backup
uevr_backup.exe --create --full

# Restore from backup
uevr_restore.exe --from-backup "backup_20240101.zip"

# Export configuration
uevr_export.exe --config --all
```

## 📚 **Next Steps**

### **After Installation**
1. **[Configuration Guide](../configuration/README.md)** - Customize your setup
2. **[First VR Game](../tutorials/first-vr-game.md)** - Launch your first VR game
3. **[Performance Tuning](../performance/README.md)** - Optimize VR experience
4. **[Troubleshooting](../troubleshooting/README.md)** - Solve common issues

### **Advanced Usage**
1. **[Plugin Development](../plugins/README.md)** - Create custom features
2. **[Adapter Development](../adapters/development/README.md)** - Build engine support
3. **[API Reference](../api/README.md)** - Programming interface
4. **[Performance Analysis](../profiling/README.md)** - Advanced optimization

## 🆘 **Support**

### **Getting Help**
- **Discord** - [UEVR Community](https://discord.gg/uevr)
- **GitHub Issues** - [Report Problems](https://github.com/uevr/uevr/issues)
- **Documentation** - [Full Documentation](../README.md)
- **Community Wiki** - [User Guides](https://github.com/uevr/uevr/wiki)

### **Installation Verification**
```bash
# Complete installation checklist
uevr_checklist.exe --installation

# Verify all components
uevr_verify.exe --components --detailed
```

---

**🎉 Congratulations! You've successfully installed UEVR!**

*Ready to transform your games into immersive VR experiences? Check out the [First VR Game Tutorial](../tutorials/first-vr-game.md) to get started!*
