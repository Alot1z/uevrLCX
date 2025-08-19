# 📥 Installation Guide

> **Complete setup and installation instructions for UEVR**

This comprehensive guide covers everything you need to install and configure UEVR for optimal VR gaming performance.

## 📋 **System Requirements**

### **Minimum Requirements**
- **OS**: Windows 10 (1903) or Windows 11
- **Architecture**: x64 (64-bit)
- **CPU**: Intel Core i5-4590 / AMD FX 8350 or better
- **RAM**: 8GB DDR3/DDR4
- **GPU**: DirectX 11 compatible with 4GB VRAM
- **Storage**: 2GB free space
- **VR Headset**: OpenVR/OpenXR compatible device

### **Recommended Requirements**
- **OS**: Windows 11 (latest updates)
- **CPU**: Intel Core i7-8700K / AMD Ryzen 7 3700X or better
- **RAM**: 16GB DDR4-3200 or better
- **GPU**: RTX 3070 / RX 6700 XT or better with 8GB+ VRAM
- **Storage**: 5GB free space on SSD
- **VR Headset**: Meta Quest 3, Valve Index, or HP Reverb G2

### **VR Headset Compatibility**
- **SteamVR**: Valve Index, HTC Vive, HP Reverb G2
- **Oculus**: Quest 2/3, Rift S, Rift CV1
- **Windows MR**: HP Reverb G2, Samsung Odyssey+
- **OpenXR**: Any OpenXR compatible headset

## 🚀 **Installation Methods**

### **Method 1: Pre-built Release (Recommended)**

#### **Step 1: Download**
1. Go to [UEVR Releases](https://github.com/praydog/UEVR/releases)
2. Download the latest `UEVR-vX.X.X.zip` file
3. Note the version number for future reference

#### **Step 2: Extract**
```bash
# Extract to a permanent location
# Recommended: C:\Program Files\UEVR\
# Or: C:\Users\[Username]\Documents\UEVR\
```

#### **Step 3: Install Dependencies**
- **Visual C++ Redistributable**: [Download here](https://aka.ms/vs/17/release/vc_redist.x64.exe)
- **DirectX Runtime**: [Download here](https://www.microsoft.com/en-us/download/details.aspx?id=35)

#### **Step 4: VR Runtime Setup**
```bash
# SteamVR Users
1. Install Steam from https://store.steampowered.com/
2. Install SteamVR from Steam Library
3. Run SteamVR and complete setup

# Oculus Users
1. Download Oculus software from https://www.oculus.com/setup/
2. Complete headset setup and calibration
3. Enable Developer Mode if needed

# Windows MR Users
1. Install Windows Mixed Reality from Microsoft Store
2. Complete headset setup
3. Calibrate room boundaries
```

### **Method 2: Build from Source**

#### **Prerequisites**
```bash
# Required Software
- Visual Studio 2022 Community (free)
- CMake 3.15 or newer
- Git for Windows
- Windows 10/11 SDK
```

#### **Build Steps**
```bash
# Clone repository
git clone --recursive https://github.com/praydog/UEVR.git
cd UEVR

# Configure with CMake
cmake -B build -G "Visual Studio 17 2022" -A x64

# Build the project
cmake --build build --config Release

# Install dependencies
cmake --build build --target install
```

## ⚙️ **Configuration Setup**

### **Initial Configuration**
1. **Run UEVR as Administrator**
   - Right-click `UEVR.exe`
   - Select "Run as administrator"

2. **First Launch Setup**
   - Select your VR headset type
   - Configure IPD (Interpupillary Distance)
   - Set render scale (start with 1.0)
   - Choose performance preset

3. **Save Configuration**
   - Click "Save & Launch"
   - Configuration saved to `%APPDATA%\UEVR\`

### **Configuration Files**

#### **Main Configuration (`uevr.ini`)**
```ini
[Framework]
MenuKey=Insert
EnableImGui=true
EnableConsole=true

[VR]
HeadsetType=Auto
IPD=64.0
RenderScale=1.0
EnableMotionControls=true

[Performance]
TargetFPS=90
QualityPreset=Balanced
EnableAFR=false
EnableDynamicResolution=true

[Input]
SnapTurnAngle=45
Deadzone=0.1
EnableHaptics=true
```

#### **Game-Specific CVars (`cvars_standard.txt`)**
```ini
# Performance optimizations
Core_r.ScreenPercentage=100.0
Core_r.MotionBlurQuality=0
Core_r.SceneColorFringeQuality=0
Core_r.TemporalAA.Upsampling=0
Core_r.Shadow.Virtual.Enable=0

# VR-specific settings
Core_r.VR.EnableStereoRendering=true
Core_r.VR.StereoMode=1
Core_r.VR.IPD=64.0
```

## 🎮 **Game Setup**

### **Supported Game Engines**
- **Unreal Engine 4.18-5.x**: Native support
- **RE Engine**: Resident Evil series
- **REDengine4**: Cyberpunk 2077, The Witcher 3
- **MT Framework**: Monster Hunter series
- **Unity Engine**: Limited support
- **CryEngine**: Experimental support

### **Game Installation**
1. **Install your game** (Steam, Epic, GOG, etc.)
2. **Verify compatibility** in [Compatibility Matrix](games/compatibility-matrix.md)
3. **Launch UEVR** before starting the game
4. **Start the game** normally
5. **Press Insert key** to open UEVR menu
6. **Enable VR mode** and adjust settings

### **Game Profiles**
UEVR automatically detects game engines and applies optimized settings:
- **Automatic detection** of engine type
- **Pre-configured profiles** for popular games
- **Custom profiles** for specific titles
- **Performance optimization** based on engine

## 🔧 **Advanced Configuration**

### **Performance Tuning**
```ini
[Performance]
# Frame rate settings
TargetFPS=90
EnableAFR=true
EnableDynamicResolution=true

# Quality settings
RenderScale=0.8
ShadowQuality=Medium
TextureQuality=High
```

### **VR Experience Settings**
```ini
[VR]
# Comfort settings
SnapTurnAngle=45
SnapTurnDeadzone=0.1
EnableVignette=true
VignetteStrength=0.3

# Visual settings
EnableChromaticAberration=false
EnableMotionBlur=false
EnableDepthOfField=false
```

### **Input Customization**
```ini
[Input]
# Controller mapping
LeftTrigger=LeftTrigger
RightTrigger=RightTrigger
LeftGrip=LeftShoulder
RightGrip=RightShoulder

# Advanced features
EnableGestureControls=true
EnableHapticFeedback=true
ControllerSwap=false
```

## 🧪 **Testing & Validation**

### **System Test**
1. **VR Runtime Test**
   - Verify headset detection
   - Check controller connectivity
   - Test room-scale tracking

2. **Performance Test**
   - Run UEVR performance benchmark
   - Verify 90+ FPS capability
   - Check GPU utilization

3. **Game Compatibility Test**
   - Launch test game
   - Verify VR activation
   - Test basic VR functionality

### **Troubleshooting Common Issues**

#### **VR Not Detected**
```bash
# Check VR runtime status
1. Ensure VR software is running
2. Check USB connections
3. Restart VR runtime
4. Run UEVR as Administrator
```

#### **Performance Issues**
```bash
# Performance optimization
1. Lower render scale (0.8 or 0.9)
2. Enable AFR mode
3. Disable post-processing
4. Update GPU drivers
```

#### **Controller Problems**
```bash
# Controller troubleshooting
1. Calibrate VR controllers
2. Check input mappings
3. Restart VR runtime
4. Verify controller firmware
```

## 📚 **Post-Installation**

### **Essential Reading**
1. **[Quick Start Guide](quick-start.md)** - Basic usage
2. **[Game Profiles](../profiles/)** - Optimized settings
3. **[Performance Guide](performance/overview.md)** - Optimization tips
4. **[Troubleshooting](reference/troubleshooting.md)** - Problem solving

### **Community Resources**
- **[Discord Server](https://discord.gg/uevr)** - Real-time support
- **[GitHub Issues](https://github.com/praydog/UEVR/issues)** - Bug reports
- **[Community Wiki](https://github.com/praydog/UEVR/wiki)** - User guides

### **Updates & Maintenance**
- **Check for updates** regularly
- **Backup configurations** before updates
- **Test after updates** with your games
- **Report issues** to help improve UEVR

## ✅ **Installation Checklist**

- [ ] **System requirements** verified
- [ ] **UEVR downloaded** and extracted
- [ ] **Dependencies installed** (VC++ Redist, DirectX)
- [ ] **VR runtime configured** and tested
- [ ] **UEVR launched** as Administrator
- [ ] **Configuration saved** and tested
- [ ] **Test game launched** and VR enabled
- [ ] **Performance verified** (90+ FPS)
- [ ] **Controllers working** and calibrated
- [ ] **Documentation reviewed** for advanced features

---

**🎉 Installation Complete! You're ready to experience VR gaming with UEVR!**

*Next: [First Steps](first-steps.md) → [Game Profiles](../profiles/) → [Performance Guide](performance/overview.md)*
