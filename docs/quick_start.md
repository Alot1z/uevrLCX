# 🚀 **UEVR Quick Start Guide**

## ⚡ **Get Started in 5 Minutes**

Welcome to UEVR! This quick start guide will have you running your first VR modification in under 5 minutes. Whether you're a developer or just want to experience VR gaming, we've got you covered.

---

## 🎯 **What You'll Learn**

- **Install UEVR** - Get the system up and running
- **Configure Your Setup** - Set up VR hardware and software
- **Run Your First VR Mod** - Experience VR gaming
- **Troubleshoot Issues** - Solve common problems
- **Next Steps** - Continue your VR journey

---

## 📋 **Prerequisites**

### **🖥️ System Requirements**

#### **Minimum Requirements:**
- **OS:** Windows 10/11, Linux (Ubuntu 20.04+), macOS 11+
- **CPU:** Intel i5-4590 / AMD FX 8350 or better
- **RAM:** 8GB RAM
- **GPU:** NVIDIA GTX 970 / AMD R9 290 or better
- **Storage:** 5GB free space
- **VR Headset:** Any OpenVR/OpenXR compatible headset

#### **Recommended Requirements:**
- **OS:** Windows 11, Linux (Ubuntu 22.04+), macOS 12+
- **CPU:** Intel i7-8700K / AMD Ryzen 7 3700X or better
- **RAM:** 16GB RAM or more
- **GPU:** NVIDIA RTX 3070 / AMD RX 6700 XT or better
- **Storage:** 10GB free space (SSD recommended)
- **VR Headset:** Meta Quest 3, Valve Index, or similar

### **🎮 VR Hardware**

#### **Supported Headsets:**
- **Meta:** Quest 2, Quest 3, Quest Pro
- **Valve:** Index, Index Controllers
- **HTC:** Vive, Vive Pro, Vive Pro 2
- **HP:** Reverb G2
- **Pimax:** 8K, 5K, Artisan
- **Windows Mixed Reality:** Various headsets

#### **Controllers:**
- **Motion Controllers** - Standard VR controllers
- **Hand Tracking** - Quest hand tracking
- **Gamepad Support** - Xbox, PlayStation, generic controllers
- **Keyboard & Mouse** - Traditional input methods

---

## 🚀 **Step 1: Installation**

### **📥 Download UEVR**

#### **Option 1: Pre-built Binary (Recommended)**
```bash
# Download latest release
# Visit: https://github.com/uevr/uevr/releases
# Download: uevr-windows-x64.zip (or your platform)
```

#### **Option 2: Build from Source**
```bash
# Clone repository
git clone https://github.com/uevr/uevr.git
cd uevr

# Build project
cmake -B build
cmake --build build --config Release
```

### **🔧 Install Dependencies**

#### **Windows:**
```bash
# Install Visual C++ Redistributable
# Download from Microsoft website

# Install SteamVR (if using SteamVR headsets)
# Download from Steam
```

#### **Linux:**
```bash
# Install system dependencies
sudo apt update
sudo apt install libvulkan1 libgl1-mesa-glx libxrandr2

# Install SteamVR (if using SteamVR headsets)
# Available in Steam
```

#### **macOS:**
```bash
# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake vulkan-headers
```

---

## ⚙️ **Step 2: Configuration**

### **🎮 VR Setup**

#### **1. Connect Your Headset:**
```bash
# Physical connection
# USB-C/USB-A for data
# DisplayPort/HDMI for video
# Power connection if required
```

#### **2. Install VR Runtime:**
```bash
# SteamVR (for SteamVR headsets)
# Download from Steam

# Oculus (for Meta headsets)
# Download from Meta website

# OpenXR (for OpenXR headsets)
# Usually pre-installed
```

#### **3. Calibrate Headset:**
```bash
# Follow headset setup wizard
# Set up room boundaries
# Calibrate controllers
# Test tracking
```

### **🔧 UEVR Configuration**

#### **1. Create Config Directory:**
```bash
# Windows
mkdir %APPDATA%\UEVR

# Linux/macOS
mkdir ~/.config/uevr
```

#### **2. Generate Configuration:**
```bash
# Run UEVR for first time
# This will create default config files
./uevr.exe --generate-config
```

#### **3. Edit Configuration:**
```bash
# Open config file
notepad %APPDATA%\UEVR\config.json

# Linux/macOS
nano ~/.config/uevr/config.json
```

#### **4. Basic Configuration:**
```json
{
  "vr": {
    "headset": "auto",
    "controllers": "auto",
    "room_scale": true,
    "performance_mode": "balanced"
  },
  "games": {
    "auto_detect": true,
    "default_vr_mode": "immersive",
    "comfort_features": true
  },
  "performance": {
    "target_fps": 90,
    "adaptive_quality": true,
    "ai_optimization": true
  }
}
```

---

## 🎮 **Step 3: Your First VR Experience**

### **🎯 Choose a Game**

#### **Recommended First Games:**
- **Cyberpunk 2077** - Open world RPG
- **Resident Evil 7** - Horror survival
- **Monster Hunter World** - Action combat
- **Any Unreal Engine Game** - Universal compatibility

### **🚀 Launch VR Mode**

#### **Method 1: Automatic Detection**
```bash
# 1. Launch your game normally
# 2. UEVR will automatically detect it
# 3. Press F11 to toggle VR mode
# 4. Enjoy VR gaming!
```

#### **Method 2: Manual Launch**
```bash
# 1. Launch UEVR first
./uevr.exe

# 2. Select your game from the list
# 3. Click "Launch in VR"
# 4. Game will start in VR mode
```

#### **Method 3: Command Line**
```bash
# Launch specific game in VR
./uevr.exe --game "Cyberpunk 2077" --vr-mode immersive
```

### **🎮 VR Controls**

#### **Basic Controls:**
- **F11** - Toggle VR mode on/off
- **F12** - VR settings menu
- **Ctrl+F11** - Quick VR restart
- **Alt+F11** - Performance overlay

#### **VR Menu Navigation:**
- **Thumbstick** - Navigate menus
- **Trigger** - Select options
- **Grip** - Back/cancel
- **Menu Button** - Open main menu

---

## 🔧 **Step 4: Troubleshooting**

### **🚨 Common Issues**

#### **Headset Not Detected:**
```bash
# Check connections
# Verify VR runtime is running
# Restart VR runtime
# Check USB drivers
# Try different USB ports
```

#### **Poor Performance:**
```bash
# Reduce graphics settings
# Enable performance mode
# Close background applications
# Update graphics drivers
# Check thermal throttling
```

#### **VR Mode Not Working:**
```bash
# Verify game compatibility
# Check UEVR logs
# Restart UEVR
# Verify game is running
# Check firewall settings
```

#### **Motion Sickness:**
```bash
# Enable comfort features
# Reduce movement speed
# Use teleportation
# Take regular breaks
# Adjust IPD settings
```

### **📊 Performance Optimization**

#### **Graphics Settings:**
```bash
# Reduce resolution
# Lower shadow quality
# Disable anti-aliasing
# Reduce draw distance
# Enable performance mode
```

#### **VR Settings:**
```bash
# Lower refresh rate (72Hz instead of 90Hz)
# Reduce FOV
# Enable motion smoothing
# Use fixed foveated rendering
# Optimize for performance
```

---

## 🎯 **Step 5: Next Steps**

### **🔧 Advanced Configuration**

#### **Custom VR Profiles:**
```bash
# Create game-specific profiles
# Optimize for each game
# Save custom settings
# Share with community
```

#### **Performance Tuning:**
```bash
# Monitor performance metrics
# Adjust quality settings
# Optimize for your hardware
# Benchmark different configurations
```

#### **Community Mods:**
```bash
# Browse community mods
# Install additional features
# Contribute your own mods
# Join development discussions
```

### **📚 Learning Resources**

#### **Documentation:**
- **User Guide** - [Complete user documentation](user_guide.md)
- **API Reference** - [Developer API documentation](api_reference.md)
- **Troubleshooting** - [Common issues and solutions](troubleshooting.md)
- **Performance Guide** - [Optimization techniques](performance.md)

#### **Community:**
- **Discord Server** - [https://discord.gg/uevr](https://discord.gg/uevr)
- **GitHub Discussions** - [github.com/uevr/uevr/discussions](https://github.com/uevr/uevr/discussions)
- **Reddit Community** - [reddit.com/r/UEVR](https://reddit.com/r/UEVR)
- **Video Tutorials** - [YouTube channel](https://youtube.com/uevr)

---

## 🎮 **Game-Specific Quick Starts**

### **🌃 Cyberpunk 2077**

#### **Quick Setup:**
```bash
# 1. Launch Cyberpunk 2077
# 2. Press F11 to enable VR
# 3. Configure VR settings
# 4. Enjoy Night City in VR!
```

#### **Recommended Settings:**
```json
{
  "vr_mode": "immersive",
  "comfort_features": true,
  "motion_controls": true,
  "performance_mode": "balanced"
}
```

### **🧟 Resident Evil 7**

#### **Quick Setup:**
```bash
# 1. Launch Resident Evil 7
# 2. Press F11 to enable VR
# 3. Enable horror comfort features
# 4. Experience survival horror in VR!
```

#### **Recommended Settings:**
```json
{
  "vr_mode": "horror_optimized",
  "comfort_features": true,
  "motion_sickness_reduction": true,
  "atmosphere_preservation": true
}
```

### **⚔️ Monster Hunter World**

#### **Quick Setup:**
```bash
# 1. Launch Monster Hunter World
# 2. Press F11 to enable VR
# 3. Configure combat controls
# 4. Hunt monsters in immersive VR!
```

#### **Recommended Settings:**
```json
{
  "vr_mode": "combat_optimized",
  "motion_controls": true,
  "combat_vr_integration": true,
  "performance_mode": "performance"
}
```

---

## 🔍 **Troubleshooting Quick Reference**

### **🚨 Emergency Solutions**

#### **VR Not Working:**
```bash
# Quick fix sequence:
1. Restart VR runtime
2. Restart UEVR
3. Restart game
4. Check connections
5. Verify compatibility
```

#### **Performance Issues:**
```bash
# Quick performance fix:
1. Enable performance mode
2. Reduce graphics settings
3. Close background apps
4. Check thermal throttling
5. Update drivers
```

#### **Motion Sickness:**
```bash
# Quick comfort fix:
1. Enable comfort features
2. Reduce movement speed
3. Use teleportation
4. Take breaks
5. Adjust IPD
```

---

## 📞 **Getting Help**

### **🆘 Support Channels**

#### **Immediate Help:**
- **Discord Server** - Real-time support
- **GitHub Issues** - Bug reports and feature requests
- **Community Forums** - User discussions and solutions

#### **Documentation:**
- **This Guide** - Quick start and basics
- **User Manual** - Complete user documentation
- **API Docs** - Developer documentation
- **Video Tutorials** - Visual learning resources

### **🤝 Community Support**

#### **How to Get Help:**
1. **Check Documentation** - Often solves the issue
2. **Search Issues** - See if others had the same problem
3. **Ask in Discord** - Get real-time help
4. **Create Issue** - Report bugs or request features
5. **Join Discussions** - Participate in community conversations

---

## 🎉 **Congratulations!**

### **🌟 You're Ready for VR Gaming!**

You've successfully:
- ✅ **Installed UEVR** - System is up and running
- ✅ **Configured VR** - Hardware and software are ready
- ✅ **Launched VR Mode** - Experienced your first VR game
- ✅ **Troubleshot Issues** - Learned to solve problems
- ✅ **Found Resources** - Know where to get help

### **🚀 What's Next?**

#### **Immediate:**
- **Try Different Games** - Explore VR compatibility
- **Customize Settings** - Optimize for your preferences
- **Join Community** - Connect with other VR enthusiasts

#### **Short-term:**
- **Learn Advanced Features** - Master UEVR capabilities
- **Create Custom Profiles** - Optimize for specific games
- **Share Experiences** - Help other users

#### **Long-term:**
- **Contribute to Development** - Help improve UEVR
- **Create Mods** - Build new VR features
- **Join Beta Testing** - Test new features early

---

## 📚 **Additional Resources**

### **🔗 Quick Links**

- **Main Documentation:** [docs.uevr.dev](https://docs.uevr.dev)
- **Community Discord:** [discord.gg/uevr](https://discord.gg/uevr)
- **GitHub Repository:** [github.com/uevr/uevr](https://github.com/uevr/uevr)
- **Issue Tracker:** [github.com/uevr/uevr/issues](https://github.com/uevr/uevr/issues)

### **📖 Next Reading**

- **User Guide** - Complete user documentation
- **Performance Guide** - Optimization techniques
- **Developer Guide** - Building mods and extensions
- **Community Guide** - Getting involved in the community

---

*UEVR Quick Start Guide - Get Started with VR Gaming in Minutes*
*Last updated: August 2024*
