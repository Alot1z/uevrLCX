# Frequently Asked Questions (FAQ)

This document answers the most common questions about UEVR Universal Engine VR Adapter. If you don't find your answer here, check the [Troubleshooting Guide](troubleshooting.md) or create a GitHub issue.

## 🚀 **Getting Started**

### **What is UEVR?**
UEVR (Universal Engine VR Adapter) is a revolutionary cross-engine VR injection framework that enables virtual reality support in games not originally designed for VR. It works by intercepting and modifying game rendering at runtime to provide stereoscopic 3D vision and VR input support.

### **Which game engines does UEVR support?**
UEVR supports multiple game engines including:
- **Unreal Engine 4/5** - Full native support
- **MT Framework** - Capcom games (Monster Hunter, Resident Evil)
- **RE Engine** - Resident Evil 7/8, Devil May Cry 5
- **REDengine 4** - Cyberpunk 2077, The Witcher 3
- **Unity Engine** - Limited support for specific versions
- **Custom engines** - Through the adapter system

### **What VR headsets are supported?**
UEVR supports all major VR platforms:
- **OpenVR** - HTC Vive, Valve Index, Oculus Rift (via SteamVR)
- **OpenXR** - Windows Mixed Reality, Oculus Quest (via Link/Air Link)
- **SteamVR** - All SteamVR-compatible headsets

### **What are the system requirements?**
**Minimum Requirements:**
- Windows 10/11 (64-bit)
- DirectX 11 compatible GPU
- 8GB RAM
- VR headset and compatible controllers

**Recommended Requirements:**
- Windows 11 (64-bit)
- RTX 3060 or equivalent
- 16GB RAM
- SSD storage
- High-end VR headset

## 📥 **Installation & Setup**

### **How do I install UEVR?**
1. Download the latest release from [GitHub Releases](https://github.com/praydog/uevr/releases)
2. Extract the ZIP file to a folder
3. Run `UEVR.exe` as Administrator
4. Follow the setup wizard

### **Why do I need to run as Administrator?**
UEVR requires administrator privileges to:
- Inject DLLs into game processes
- Modify game memory for VR rendering
- Access system-level graphics APIs
- Install system hooks

### **What if the installer fails?**
Common solutions:
- **Antivirus blocking**: Add UEVR to exclusions
- **Missing dependencies**: Install Visual C++ Redistributables
- **Permission issues**: Ensure you're running as Administrator
- **Corrupted download**: Re-download the installer

### **Do I need to install VR runtimes separately?**
Yes, you need to install:
- **SteamVR** for OpenVR headsets
- **Oculus App** for Oculus headsets
- **Windows Mixed Reality** for WMR headsets

## 🎮 **Game Compatibility**

### **How do I know if a game is compatible?**
Check the [Compatibility Matrix](games/compatibility-matrix.md) for:
- Game engine support
- VR feature compatibility
- Known issues and workarounds
- Performance recommendations

### **Why doesn't my game work with UEVR?**
Common reasons:
- **Unsupported engine**: Game uses an engine not yet supported
- **Anti-cheat protection**: Game has anti-cheat that blocks injection
- **DRM protection**: Game has DRM that prevents modification
- **Version mismatch**: Game version not compatible with current UEVR

### **Can I play multiplayer games with UEVR?**
**Single-player games**: Full VR support
**Co-op games**: Usually work, but may have issues
**Competitive multiplayer**: Not recommended due to:
- Potential anti-cheat detection
- Fair play concerns
- Risk of account bans

### **What if a game crashes with UEVR?**
1. Check the [Troubleshooting Guide](troubleshooting.md)
2. Verify game compatibility
3. Try different VR settings
4. Check UEVR logs for error messages
5. Report the issue on GitHub

## 🔧 **Configuration & Settings**

### **How do I configure VR settings?**
1. Launch UEVR
2. Go to **Settings** → **VR Configuration**
3. Adjust:
   - **IPD** (Interpupillary Distance)
   - **Render Scale** (Resolution multiplier)
   - **Frame Rate** (Target FPS)
   - **Graphics Quality** (Performance vs. quality)

### **What are the best VR settings for performance?**
**Performance-focused:**
- Render Scale: 0.8x
- Frame Rate: 72 FPS
- Graphics Quality: Low
- Disable advanced features

**Quality-focused:**
- Render Scale: 1.2x
- Frame Rate: 90 FPS
- Graphics Quality: High
- Enable advanced features

### **How do I save my configuration?**
UEVR automatically saves settings to:
- `%APPDATA%\UEVR\config.ini`
- `%APPDATA%\UEVR\profiles\`

You can also:
- Export/import configurations
- Create game-specific profiles
- Share configurations with others

### **Can I use different settings for different games?**
Yes! UEVR supports:
- **Game profiles**: Automatic settings per game
- **Preset profiles**: Performance, Quality, Balanced
- **Custom profiles**: User-defined configurations
- **Auto-detection**: Smart settings based on game requirements

## 🎯 **VR Experience**

### **How do I control games in VR?**
UEVR provides multiple input methods:
- **VR Controllers**: Full 6DOF motion control
- **Gamepad**: Traditional controller support
- **Keyboard/Mouse**: Standard PC input
- **Hybrid**: Combine multiple input methods

### **What VR interactions are supported?**
- **Head tracking**: Look around in 3D space
- **Hand presence**: See your hands/controllers
- **Motion control**: Use VR controllers for actions
- **Room scale**: Move around in physical space
- **Seated/Standing**: Multiple play styles

### **How do I adjust the VR camera?**
In-game adjustments:
- **Reset View**: Recenter the camera
- **Height Offset**: Adjust for sitting/standing
- **Forward Offset**: Move camera forward/back
- **Tilt Adjustment**: Fix camera angle

### **What if the VR view feels wrong?**
Common fixes:
- **IPD adjustment**: Match your eye distance
- **Height calibration**: Set correct standing height
- **Room setup**: Proper VR space configuration
- **Game-specific settings**: Adjust per-game camera

## 🚨 **Troubleshooting**

### **UEVR won't start**
**Check these:**
1. **Administrator rights**: Run as Administrator
2. **Antivirus**: Add to exclusions
3. **Dependencies**: Install Visual C++ Redistributables
4. **Windows version**: Ensure Windows 10/11
5. **Graphics drivers**: Update to latest version

### **VR not detected**
**Verify:**
1. **VR runtime**: SteamVR/Oculus running
2. **Headset connection**: Properly connected
3. **USB ports**: Try different USB ports
4. **Drivers**: Update VR headset drivers
5. **Windows settings**: Enable VR in Windows

### **Game crashes when launching**
**Try these steps:**
1. **Verify game files**: Steam/Epic verification
2. **Update game**: Install latest patches
3. **Check compatibility**: Verify game support
4. **Reduce settings**: Lower VR quality settings
5. **Clean launch**: Close other applications

### **Poor VR performance**
**Optimization steps:**
1. **Lower render scale**: Reduce resolution
2. **Disable features**: Turn off advanced VR features
3. **Update drivers**: Latest graphics drivers
4. **Close background apps**: Free up system resources
5. **Check thermal**: Ensure proper cooling

### **VR input not working**
**Troubleshooting:**
1. **Controller pairing**: Re-pair VR controllers
2. **Input mapping**: Check UEVR input settings
3. **Game compatibility**: Verify input support
4. **Controller batteries**: Ensure sufficient charge
5. **SteamVR settings**: Verify controller configuration

## 🔧 **Development & Customization**

### **How do I create custom adapters?**
See the [Custom Adapters Guide](adapters/custom-adapters.md) for:
- Adapter architecture overview
- Step-by-step implementation
- Code examples and templates
- Testing and validation

### **Can I modify UEVR source code?**
Yes! UEVR is open source:
- **Fork the repository** on GitHub
- **Make your modifications**
- **Test thoroughly**
- **Submit a pull request**

### **How do I debug UEVR issues?**
Debugging tools:
- **UEVR logs**: Check log files for errors
- **Debug builds**: Use debug version for detailed info
- **Performance monitoring**: Built-in performance tools
- **Error reporting**: Automatic error collection

### **What programming languages are supported?**
- **C++**: Primary development language
- **Lua**: Scripting and automation
- **Python**: Build scripts and tools
- **Batch/PowerShell**: Windows automation

## 📊 **Performance & Optimization**

### **What affects VR performance?**
Key factors:
- **GPU power**: Graphics card capability
- **CPU performance**: Processor speed
- **Memory**: RAM amount and speed
- **Storage**: SSD vs. HDD
- **VR settings**: Quality vs. performance trade-offs

### **How do I optimize for my hardware?**
**Low-end systems:**
- Use performance presets
- Reduce render scale
- Disable advanced features
- Close background applications

**High-end systems:**
- Enable quality presets
- Increase render scale
- Use advanced features
- Enable performance monitoring

### **What's the difference between 72Hz and 90Hz?**
- **72Hz**: Lower performance requirement, smoother on lower-end systems
- **90Hz**: Higher performance requirement, more responsive VR experience
- **120Hz**: Maximum performance requirement, ultra-smooth experience

### **How do I monitor performance?**
UEVR provides:
- **Real-time FPS**: Current frame rate
- **Performance graphs**: Visual performance data
- **Resource usage**: CPU, GPU, memory usage
- **VR metrics**: Latency, reprojection rate

## 🔒 **Security & Safety**

### **Is UEVR safe to use?**
Yes, UEVR is designed with security in mind:
- **Open source**: Code is publicly auditable
- **No telemetry**: No data collection
- **Local processing**: Everything runs locally
- **Community tested**: Widely used and verified

### **Can UEVR get me banned?**
**Single-player games**: Very low risk
**Multiplayer games**: Higher risk due to:
- Anti-cheat detection
- Fair play policies
- Terms of service violations

**Recommendation**: Use only in single-player or private multiplayer

### **Does UEVR collect personal data?**
No, UEVR:
- **Runs locally**: No internet communication
- **No telemetry**: No data collection
- **No analytics**: No usage tracking
- **Privacy-focused**: Designed for user privacy

### **What if my antivirus flags UEVR?**
This is normal because UEVR:
- **Modifies game processes**: Legitimate VR functionality
- **Uses DLL injection**: Required for VR support
- **Requires admin rights**: System-level access needed

**Solution**: Add UEVR to antivirus exclusions

## 🔄 **Updates & Maintenance**

### **How often is UEVR updated?**
Update schedule:
- **Major releases**: Every 3-6 months
- **Minor releases**: Monthly
- **Patch releases**: Weekly
- **Beta releases**: Continuous development

### **How do I update UEVR?**
**Automatic updates:**
1. Enable auto-updates in settings
2. UEVR checks for updates on launch
3. Download and install automatically

**Manual updates:**
1. Download latest release
2. Backup your configuration
3. Install new version
4. Restore configuration

### **Should I backup my configuration?**
Yes, always backup:
- **User profiles**: Game-specific settings
- **Custom configurations**: Personal preferences
- **Lua scripts**: Custom automation
- **VR settings**: Calibration data

### **What if an update breaks something?**
**Recovery options:**
1. **Rollback**: Install previous version
2. **Reset settings**: Use default configuration
3. **Check compatibility**: Verify game support
4. **Report issue**: Create GitHub issue

## 🌐 **Community & Support**

### **Where can I get help?**
**Support channels:**
- **GitHub Issues**: Bug reports and feature requests
- **Discord Server**: Real-time community support
- **Community Wiki**: User-maintained knowledge base
- **Reddit**: r/UEVR community discussions

### **How do I report bugs?**
**Bug report template:**
1. **UEVR version**: Current version number
2. **Game details**: Name, engine, version
3. **System info**: OS, hardware, drivers
4. **Steps to reproduce**: Clear reproduction steps
5. **Expected vs. actual**: What should happen vs. what happens
6. **Logs**: Attach relevant log files

### **Can I contribute to UEVR?**
Yes! Contributions welcome:
- **Code**: Bug fixes and new features
- **Documentation**: Improve guides and tutorials
- **Testing**: Test on different systems
- **Community**: Help other users

### **How do I join the community?**
**Get involved:**
1. **Join Discord**: [UEVR Discord Server](https://discord.gg/uevr)
2. **Follow GitHub**: Star and watch the repository
3. **Share experiences**: Post about your VR gaming
4. **Help others**: Answer questions in the community

## 📚 **Additional Resources**

### **Where can I learn more?**
**Learning resources:**
- [Quick Start Guide](quick-start.md) - Get started quickly
- [Installation Guide](installation.md) - Complete setup
- [API Reference](development/api-reference.md) - Developer documentation
- [Troubleshooting Guide](troubleshooting.md) - Problem solving
- [Performance Guide](performance/overview.md) - Optimization tips

### **Are there video tutorials?**
Yes, check:
- **YouTube**: UEVR official channel
- **Twitch**: Community streams
- **Discord**: Video tutorials and guides
- **Community**: User-created content

### **What's the best way to learn UEVR?**
**Learning path:**
1. **Start simple**: Basic VR setup
2. **Experiment**: Try different games and settings
3. **Read documentation**: Understand the system
4. **Join community**: Learn from others
5. **Contribute**: Help improve UEVR

---

## 🆘 **Still Need Help?**

If you couldn't find your answer here:

1. **Check the [Troubleshooting Guide](troubleshooting.md)**
2. **Search existing [GitHub Issues](https://github.com/praydog/uevr/issues)**
3. **Ask in the [Discord Community](https://discord.gg/uevr)**
4. **Create a new GitHub issue** with detailed information

---

*Last updated: January 15, 2024*  
*FAQ version: 2.0.0*
