# 🚀 Quick Start Guide

> **Get up and running with UEVR in minutes!**

This guide will walk you through the essential steps to get UEVR working with your favorite Unreal Engine games in virtual reality.

## ⚡ **5-Minute Setup**

### **Step 1: Download UEVR**
1. Go to [UEVR Releases](https://github.com/praydog/UEVR/releases)
2. Download the latest `UEVR.zip` file
3. Extract to a folder (e.g., `C:\UEVR`)

### **Step 2: Install VR Runtime**
- **SteamVR Users**: Install Steam and SteamVR
- **Oculus Users**: Install Oculus software
- **Windows MR**: Install Windows Mixed Reality

### **Step 3: Launch UEVR**
1. Run `UEVR.exe` as Administrator
2. Configure your VR headset settings
3. Click "Save & Launch"

### **Step 4: Launch Your Game**
1. Start your favorite Unreal Engine game
2. Press `Insert` key to open UEVR menu
3. Enable VR mode
4. Enjoy VR gaming!

## 🎮 **First VR Experience**

### **Recommended First Games**
- **Cyberpunk 2077**: Excellent VR experience with motion controls
- **Monster Hunter World**: Immersive hunting in VR
- **Resident Evil 7/8**: Horror games perfect for VR
- **The Witcher 3**: Open world RPG in virtual reality

### **Essential VR Settings**
- **Stereo Mode**: Enable for proper 3D vision
- **Motion Controls**: Enable for hand tracking
- **Snap Turn**: Enable for comfortable movement
- **Performance Mode**: Start with "Balanced" setting

## ⚙️ **Basic Configuration**

### **VR Headset Setup**
```ini
[VR]
HeadsetType=Auto
IPD=64.0
RenderScale=1.0
```

### **Performance Settings**
```ini
[Performance]
TargetFPS=90
QualityPreset=Balanced
EnableAFR=false
```

### **Input Configuration**
```ini
[Input]
EnableMotionControls=true
SnapTurnAngle=45
Deadzone=0.1
```

## 🔧 **Troubleshooting Common Issues**

### **VR Not Detected**
- Ensure VR runtime is running
- Check USB connections
- Restart UEVR as Administrator

### **Poor Performance**
- Lower render scale
- Disable post-processing
- Enable AFR mode
- Check GPU drivers

### **Controller Issues**
- Calibrate VR controllers
- Check input mappings
- Restart VR runtime

## 📚 **Next Steps**

After completing this quick start:

1. **[Read the Full Documentation](README.md)**
2. **[Explore Game Profiles](../profiles/)**
3. **[Join our Discord](https://discord.gg/uevr)**
4. **[Learn Advanced Features](advanced-features.md)**

## 🆘 **Need Help?**

- **Discord**: [UEVR Community](https://discord.gg/uevr)
- **GitHub Issues**: [Report Problems](https://github.com/praydog/UEVR/issues)
- **Wiki**: [Community Knowledge Base](https://github.com/praydog/UEVR/wiki)

---

**🎉 Congratulations! You're now ready to experience your favorite games in VR!**

*Next: [Installation Guide](installation.md) → [First Steps](first-steps.md)*
