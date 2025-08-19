# Troubleshooting Guide

This guide provides comprehensive troubleshooting information for UEVR, including common issues, solutions, diagnostic procedures, and recovery methods.

## Quick Diagnostic Checklist

### Before Starting Troubleshooting
- [ ] UEVR is properly installed and up to date
- [ ] VR runtime (SteamVR/OpenXR) is installed and running
- [ ] Graphics drivers are up to date
- [ ] Game is compatible with UEVR
- [ ] System meets minimum requirements

### Basic System Check
- [ ] Windows 10/11 (64-bit)
- [ ] 16GB+ RAM
- [ ] DirectX 11/12 or Vulkan compatible GPU
- [ ] Latest graphics drivers
- [ ] SteamVR or OpenXR runtime

## Common Issues and Solutions

### 1. UEVR Won't Start

#### Symptoms
- UEVR launcher doesn't open
- "Failed to initialize" error
- Application crashes immediately

#### Possible Causes
- Missing dependencies
- Corrupted installation
- Antivirus blocking
- Insufficient permissions

#### Solutions
```bash
# 1. Check dependencies
# Ensure Visual C++ Redistributables are installed
# Download from Microsoft: https://aka.ms/vs/17/release/vc_redist.x64.exe

# 2. Reinstall UEVR
# Download latest version from GitHub
# Extract to a clean directory
# Run as Administrator

# 3. Check antivirus
# Add UEVR directory to antivirus exclusions
# Temporarily disable antivirus for testing

# 4. Check Windows permissions
# Right-click UEVR.exe → Properties → Compatibility
# Check "Run this program as an administrator"
```

#### Diagnostic Commands
```bash
# Check system requirements
systeminfo | findstr /C:"Total Physical Memory"
wmic path win32_VideoController get name

# Check for missing DLLs
drmemory -- UEVR.exe

# Check Windows Event Viewer
eventvwr.msc
# Look for Application errors related to UEVR
```

### 2. VR Not Detected

#### Symptoms
- "No VR headset detected" message
- VR mode not available
- Headset not recognized

#### Possible Causes
- VR runtime not running
- Headset not connected properly
- Driver issues
- USB/DisplayPort problems

#### Solutions
```bash
# 1. Check VR runtime
# For SteamVR:
# - Open Steam
# - Library → Tools → SteamVR
# - Install and run SteamVR
# - Check headset status in SteamVR

# For OpenXR:
# - Install Windows Mixed Reality
# - Check Windows Settings → Mixed Reality

# 2. Check headset connection
# - Disconnect and reconnect USB/DisplayPort
# - Try different USB ports
# - Check cable integrity

# 3. Update/reinstall drivers
# - Uninstall current VR drivers
# - Restart system
# - Install latest drivers from manufacturer

# 4. Check Windows settings
# - Settings → Mixed Reality → Headset display
# - Ensure headset is set as primary display
```

#### Diagnostic Commands
```bash
# Check VR runtime status
# SteamVR: Look for green status in SteamVR window
# OpenXR: Check Windows Mixed Reality settings

# Check USB devices
devmgmt.msc
# Look for VR headset under "Human Interface Devices"

# Check display adapters
wmic path win32_VideoController get name,driverversion
```

### 3. Game Crashes When VR is Enabled

#### Symptoms
- Game crashes immediately after enabling VR
- "Application has stopped working" error
- Game freezes or becomes unresponsive

#### Possible Causes
- Incompatible game version
- Memory issues
- Graphics driver problems
- UEVR compatibility issues

#### Solutions
```bash
# 1. Check game compatibility
# Verify game is in UEVR compatibility matrix
# Check if game version is supported
# Look for known compatibility issues

# 2. Memory optimization
# Close unnecessary applications
# Increase virtual memory
# Check for memory leaks

# 3. Graphics settings
# Lower in-game graphics settings
# Disable advanced features (ray tracing, DLSS)
# Set render scale to 0.8-0.9

# 4. UEVR settings
# Disable performance-intensive features
# Use conservative VR settings
# Enable compatibility mode if available
```

#### Diagnostic Commands
```bash
# Check memory usage
taskmgr
# Monitor memory usage while launching game

# Check graphics settings
# In-game: Lower all settings to minimum
# UEVR: Use safe preset

# Check crash logs
# Look in game directory for crash logs
# Check Windows Event Viewer for errors
```

### 4. Poor VR Performance

#### Symptoms
- Low frame rate in VR
- Stuttering and lag
- Motion sickness due to poor performance
- High latency

#### Possible Causes
- Insufficient hardware
- High graphics settings
- Background processes
- Driver issues

#### Solutions
```bash
# 1. Performance optimization
# Lower UEVR render scale (0.7-0.8)
# Enable Alternate Frame Rendering (AFR)
# Enable dynamic resolution scaling
# Disable unnecessary VR features

# 2. System optimization
# Close background applications
# Disable Windows Game Mode
# Update graphics drivers
# Check for thermal throttling

# 3. Game optimization
# Lower in-game graphics settings
# Disable advanced features
# Use performance presets
# Reduce draw distance and shadows

# 4. VR runtime optimization
# SteamVR: Lower supersampling
# OpenXR: Use performance mode
# Disable motion smoothing
```

#### Performance Monitoring
```bash
# Monitor frame rate
# UEVR performance overlay
# SteamVR performance graph
# Windows Game Bar

# Monitor system resources
taskmgr
# CPU, GPU, Memory usage

# Monitor temperatures
# Use tools like HWiNFO or MSI Afterburner
# Check for thermal throttling
```

### 5. VR Input Not Working

#### Symptoms
- Controllers not detected
- Input not responding
- Wrong button mappings
- Haptic feedback not working

#### Possible Causes
- Controller pairing issues
- Driver problems
- UEVR input configuration
- Runtime issues

#### Solutions
```bash
# 1. Controller setup
# Re-pair controllers with headset
# Check controller batteries
# Reset controller settings
# Update controller firmware

# 2. Input configuration
# Check UEVR input settings
# Verify button mappings
# Test input in VR runtime
# Reset input configuration

# 3. Driver issues
# Update/reinstall controller drivers
# Check Windows device manager
# Restart VR runtime
# Restart system

# 4. Runtime troubleshooting
# SteamVR: Reset controller bindings
# OpenXR: Check input settings
# Test in other VR applications
```

#### Input Testing
```bash
# Test controllers in VR runtime
# SteamVR: Controller test panel
# OpenXR: Input test application

# Check Windows recognition
devmgmt.msc
# Look for controllers under "Human Interface Devices"

# Test in other VR apps
# Verify controllers work in other applications
```

### 6. Visual Artifacts and Glitches

#### Symptoms
- Screen tearing
- Black screens
- Distorted graphics
- Flickering
- Wrong colors

#### Possible Causes
- Graphics driver issues
- Incorrect VR settings
- Hardware problems
- Compatibility issues

#### Solutions
```bash
# 1. Graphics driver fixes
# Update to latest drivers
# Clean install drivers
# Rollback to previous stable version
# Check for beta drivers

# 2. VR settings adjustment
# Adjust render scale
# Change VR rendering mode
# Disable advanced features
# Use compatibility mode

# 3. Hardware troubleshooting
# Check cable connections
# Try different ports
# Test with different headset
# Check GPU health

# 4. Software compatibility
# Update UEVR to latest version
# Check game compatibility
# Use recommended settings
# Enable safe mode if available
```

#### Visual Diagnostics
```bash
# Check graphics card health
# Use GPU monitoring tools
# Check temperatures and clock speeds
# Run GPU stress tests

# Check VR display settings
# Verify headset resolution
# Check refresh rate settings
# Test different display modes
```

## Advanced Troubleshooting

### 1. System Diagnostics

#### Performance Analysis
```bash
# Comprehensive system check
# Run Windows Performance Toolkit
# Analyze system bottlenecks
# Check for resource conflicts

# Memory diagnostics
mdsched.exe
# Run Windows Memory Diagnostic

# Disk health check
wmic diskdrive get status
# Check for disk errors and health
```

#### Driver Analysis
```bash
# Driver verification
verifier.exe
# Enable driver verification for troubleshooting

# Driver rollback
# Device Manager → Properties → Driver → Roll Back
# Restore previous working driver

# Clean driver installation
# Use DDU (Display Driver Uninstaller)
# Remove all graphics drivers
# Install fresh drivers
```

### 2. UEVR Diagnostics

#### Log Analysis
```bash
# Enable detailed logging
# Set UEVR log level to Debug
# Check log files for errors
# Look for specific error messages

# Performance profiling
# Enable UEVR performance monitoring
# Check frame time analysis
# Monitor resource usage
# Identify performance bottlenecks
```

#### Configuration Testing
```bash
# Test different configurations
# Try minimal configuration
# Test with different games
# Compare performance between settings
# Document working configurations
```

### 3. Network and Online Issues

#### Multiplayer Problems
```bash
# Check network connectivity
# Test internet connection
# Check firewall settings
# Verify game server status
# Test with different networks
```

#### Update Issues
```bash
# Check for UEVR updates
# Verify download integrity
# Check antivirus interference
# Test with different browsers
# Use alternative download methods
```

## Recovery Procedures

### 1. Complete Reset

#### UEVR Reset
```bash
# 1. Backup configuration
# Copy UEVR config files
# Save custom settings
# Document working configurations

# 2. Remove UEVR
# Uninstall UEVR
# Delete remaining files
# Clean registry entries
# Restart system

# 3. Fresh installation
# Download latest version
# Install to clean directory
# Use default settings
# Test basic functionality
```

#### System Reset
```bash
# Windows reset options
# Settings → Update & Security → Recovery
# Choose "Reset this PC"
# Select "Keep my files" or "Remove everything"
# Follow reset process
```

### 2. Configuration Recovery

#### Restore Working Config
```bash
# 1. Identify working configuration
# Check backup files
# Review configuration history
# Test different settings

# 2. Apply configuration
# Copy working config files
# Restore registry settings
# Apply saved settings
# Test functionality

# 3. Validate recovery
# Test all features
# Verify performance
# Check stability
# Document working state
```

### 3. Data Recovery

#### Save Game Recovery
```bash
# 1. Locate save files
# Check game directories
# Look in user folders
# Search for backup files

# 2. Restore saves
# Copy save files to game directory
# Verify file integrity
# Test save loading
# Check save compatibility
```

## Prevention and Maintenance

### 1. Regular Maintenance

#### System Maintenance
```bash
# Weekly tasks
# Update Windows
# Update drivers
# Clean temporary files
# Check disk health

# Monthly tasks
# Deep system cleanup
# Driver verification
# Performance optimization
# Backup important data
```

#### UEVR Maintenance
```bash
# Regular updates
# Check for UEVR updates
# Update compatible games
# Review compatibility matrix
# Test new features

# Configuration backup
# Backup working configs
# Document changes
# Test modifications
# Keep configuration history
```

### 2. Performance Monitoring

#### Continuous Monitoring
```bash
# Performance tracking
# Monitor frame rates
# Track resource usage
# Log performance changes
# Identify trends

# Alert system
# Set performance thresholds
# Monitor for degradation
# Alert on issues
# Track performance history
```

### 3. Compatibility Management

#### Game Compatibility
```bash
# Compatibility tracking
# Test new games
# Document compatibility
# Track game updates
# Update compatibility matrix

# Issue reporting
# Report compatibility issues
# Document workarounds
# Share solutions
# Contribute to community
```

## Getting Help

### 1. Self-Help Resources

#### Documentation
- **UEVR Documentation**: Check the docs/ directory
- **GitHub Wiki**: Community-maintained knowledge base
- **FAQ Section**: Common questions and answers
- **Video Tutorials**: Step-by-step guides

#### Community Resources
- **Discord Server**: Real-time help and support
- **Reddit Community**: Discussion and troubleshooting
- **Steam Community**: Game-specific help
- **YouTube Channels**: Tutorials and guides

### 2. Professional Support

#### When to Seek Professional Help
- Hardware failures
- Complex driver issues
- System corruption
- Advanced performance problems
- Compatibility issues beyond scope

#### Support Channels
- **GitHub Issues**: Bug reports and feature requests
- **Community Forums**: Peer support and discussion
- **Developer Support**: Direct support for complex issues
- **Professional Services**: Paid support for businesses

### 3. Issue Reporting

#### Effective Issue Reports
```bash
# Required information
# UEVR version
# Game name and version
# System specifications
# Error messages
# Steps to reproduce
# Expected vs actual behavior

# Additional details
# Screenshots or videos
# Log files
# Configuration files
# Recent changes
# Other troubleshooting steps
```

#### Bug Report Template
```markdown
## Issue Description
Brief description of the problem

## Steps to Reproduce
1. Step 1
2. Step 2
3. Step 3

## Expected Behavior
What should happen

## Actual Behavior
What actually happens

## System Information
- OS: Windows 10/11
- UEVR Version: X.X.X
- Game: Name Version
- Hardware: CPU, GPU, RAM

## Additional Information
Logs, screenshots, etc.
```

## Resources

- [UEVR Installation Guide](../installation.md)
- [Performance Optimization](../performance/overview.md)
- [Game Compatibility Matrix](../games/compatibility-matrix.md)
- [Development Setup](../development/setup.md)
- [API Reference](../development/api-reference.md)

---

*For additional troubleshooting support, check the GitHub Issues or join the community Discord.*
