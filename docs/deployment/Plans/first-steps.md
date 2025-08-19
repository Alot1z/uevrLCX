# 🎮 First Steps

> **Your first VR experience with UEVR**

This guide will walk you through your first VR gaming session with UEVR, from launching your first game to experiencing the magic of virtual reality.

## 🚀 **Before You Begin**

### **Prerequisites Checklist**
- ✅ UEVR installed and configured
- ✅ VR headset connected and calibrated
- ✅ VR runtime running (SteamVR, Oculus, etc.)
- ✅ Game installed and ready to launch
- ✅ Comfortable play space cleared

### **Recommended First Game**
For your first VR experience, we recommend starting with:
- **Monster Hunter World** - Excellent VR implementation, comfortable movement
- **Resident Evil 7** - Immersive horror experience
- **Cyberpunk 2077** - Open world exploration

## 🎯 **Step 1: Launch UEVR**

### **Start UEVR Application**
1. **Run as Administrator**
   - Right-click `UEVR.exe`
   - Select "Run as administrator"
   - This ensures proper hooking capabilities

2. **Verify VR Detection**
   - Check that your headset is detected
   - Verify controller connectivity
   - Confirm room-scale tracking is working

3. **Save Configuration**
   - Review your VR settings
   - Click "Save & Launch"
   - UEVR will minimize to system tray

## 🎮 **Step 2: Launch Your Game**

### **Game Launch Process**
1. **Start Your Game**
   - Launch from Steam, Epic, or other launcher
   - Wait for the game to fully load
   - Don't start playing yet!

2. **Wait for UEVR Detection**
   - UEVR automatically detects the game engine
   - Look for UEVR overlay in the top-left corner
   - Engine type should be displayed (e.g., "MT Framework", "RE Engine")

3. **Verify VR Readiness**
   - Check that UEVR shows "Ready" status
   - Ensure no error messages are displayed
   - Confirm VR headset is still connected

## 🥽 **Step 3: Enable VR Mode**

### **Open UEVR Menu**
1. **Press Insert Key**
   - This opens the UEVR configuration menu
   - Menu appears as an overlay on your screen
   - Use mouse to navigate the interface

2. **Navigate to VR Settings**
   - Click on "VR" tab in the menu
   - Look for "Enable VR" checkbox
   - Ensure "Stereo Rendering" is enabled

3. **Enable VR Mode**
   - Check the "Enable VR" checkbox
   - Your screen should immediately switch to VR mode
   - You'll see the game in your VR headset

## 🎨 **Step 4: Configure VR Experience**

### **Essential VR Settings**
```ini
[VR]
# Basic VR Configuration
EnableVR=true
StereoRendering=true
MotionControls=true

# Comfort Settings
SnapTurn=true
SnapTurnAngle=45
Vignette=true
VignetteStrength=0.3

# Visual Quality
RenderScale=1.0
Supersampling=1.0
```

### **Performance Settings**
```ini
[Performance]
# Frame Rate
TargetFPS=90
EnableAFR=false

# Quality
QualityPreset=Balanced
DynamicResolution=true
```

### **Input Configuration**
```ini
[Input]
# Controller Setup
EnableMotionControls=true
SnapTurnAngle=45
Deadzone=0.1
EnableHaptics=true
```

## 🎯 **Step 5: Your First VR Session**

### **Getting Comfortable**
1. **Start Slowly**
   - Begin with stationary gameplay
   - Avoid rapid movements initially
   - Take breaks if you feel discomfort

2. **Test Basic Functions**
   - Look around using head movement
   - Test controller tracking
   - Verify stereo 3D effect

3. **Explore Your Environment**
   - Move around in the game world
   - Test different movement speeds
   - Experiment with controller inputs

### **VR Comfort Tips**
- **Take Regular Breaks**: Every 15-20 minutes
- **Stay Hydrated**: VR can be dehydrating
- **Good Posture**: Maintain comfortable seating/standing position
- **Proper Lighting**: Ensure adequate room lighting
- **Clear Space**: Remove obstacles from play area

## 🔧 **Step 6: Troubleshooting First Issues**

### **Common First-Time Problems**

#### **VR Not Activating**
```bash
# Troubleshooting Steps
1. Check UEVR menu shows "VR Ready"
2. Verify "Enable VR" is checked
3. Restart UEVR and game
4. Check VR runtime status
```

#### **Poor Performance**
```bash
# Performance Optimization
1. Lower render scale to 0.8
2. Enable AFR mode
3. Disable post-processing
4. Check GPU utilization
```

#### **Controller Issues**
```bash
# Controller Troubleshooting
1. Recalibrate VR controllers
2. Check input mappings
3. Restart VR runtime
4. Verify controller firmware
```

#### **Motion Sickness**
```bash
# Comfort Improvements
1. Enable snap turning
2. Reduce movement speed
3. Enable vignette effect
4. Take frequent breaks
```

## 🎮 **Step 7: Advanced VR Features**

### **Motion Controls**
- **Weapon Handling**: Natural weapon manipulation
- **Gesture Controls**: Hand gestures for actions
- **Haptic Feedback**: Tactile response for immersion

### **Room Scale Movement**
- **Physical Walking**: Move around in real space
- **Boundary System**: Safe play area definition
- **Seated Mode**: Comfortable seated gameplay

### **Customization Options**
- **UI Scaling**: Adjust interface size for VR
- **Camera Settings**: Modify FOV and positioning
- **Input Mapping**: Customize controller bindings

## 📊 **Step 8: Performance Monitoring**

### **Frame Rate Monitoring**
1. **Enable Performance Overlay**
   - Check "Show FPS" in UEVR menu
   - Monitor frame rate in VR
   - Target 90+ FPS for smooth experience

2. **Performance Metrics**
   - **GPU Utilization**: Should be below 90%
   - **Frame Time**: Target 11ms or less
   - **Dropped Frames**: Should be minimal

3. **Optimization Tips**
   - Lower render scale if needed
   - Enable AFR for performance
   - Disable unnecessary effects

## 🎯 **Step 9: Game-Specific Tips**

### **Monster Hunter World**
- **Camera Mode**: Switch to first-person view
- **Weapon Handling**: Use motion controls for weapons
- **Movement**: Enable snap turning for comfort

### **Resident Evil 7**
- **Horror Experience**: Perfect for VR immersion
- **Inventory Management**: Use motion controls
- **Exploration**: Take time to explore environments

### **Cyberpunk 2077**
- **Open World**: Immersive city exploration
- **Combat**: Motion-controlled weapon handling
- **Story**: Enhanced narrative experience

## 🔄 **Step 10: Refining Your Experience**

### **Iterative Improvement**
1. **Adjust Settings Gradually**
   - Start with default settings
   - Make small adjustments
   - Test changes before proceeding

2. **Personal Comfort**
   - Find your optimal IPD setting
   - Adjust render scale for performance
   - Customize input sensitivity

3. **Game Optimization**
   - Learn game-specific settings
   - Optimize for your hardware
   - Balance quality vs. performance

## 📚 **Next Steps**

### **Immediate Actions**
1. **Practice Basic VR Movement**
2. **Test Different Games**
3. **Customize Your Settings**
4. **Join the Community**

### **Advanced Learning**
1. **[Performance Guide](performance/overview.md)** - Optimization techniques
2. **[Game Profiles](../profiles/)** - Pre-configured settings
3. **[Plugin Development](development/plugins.md)** - Create custom features
4. **[Lua Scripting](development/lua-scripting.md)** - Advanced customization

### **Community Engagement**
- **[Discord Server](https://discord.gg/uevr)** - Real-time support
- **[GitHub Issues](https://github.com/praydog/UEVR/issues)** - Report problems
- **[Community Wiki](https://github.com/praydog/UEVR/wiki)** - Share knowledge

## 🎉 **Congratulations!**

You've successfully completed your first VR gaming session with UEVR! You've experienced:
- ✅ **VR Mode Activation** - Successfully enabled VR
- ✅ **Motion Controls** - Experienced VR input
- ✅ **Stereo 3D** - Immersive visual experience
- ✅ **Performance Optimization** - Smooth VR gameplay
- ✅ **Basic Troubleshooting** - Problem-solving skills

## 🚀 **What's Next?**

### **Short Term (Next Week)**
- Try different games and genres
- Experiment with VR settings
- Join the UEVR community
- Report any issues you encounter

### **Medium Term (Next Month)**
- Master advanced VR features
- Create custom configurations
- Explore plugin development
- Contribute to the project

### **Long Term (Ongoing)**
- Stay updated with UEVR releases
- Help other users get started
- Contribute to documentation
- Participate in community development

---

**🎮 Welcome to the future of gaming! You're now part of the VR revolution!**

*Previous: [Installation Guide](installation.md) | Next: [Performance Guide](performance/overview.md) → [Game Profiles](../profiles/)*
