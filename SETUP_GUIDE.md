# uevrLCX Setup Guide

## 🚀 Quick Start

### Option 1: Automatic Installation (Recommended)

1. **Run the automatic installer:**
   ```cmd
   install_tools_wrapper.bat
   ```
   This will install all necessary development tools automatically.

2. **Build the project:**
   ```cmd
   build.bat
   ```

### Option 2: Manual Installation

If the automatic installer doesn't work, follow these manual steps:

## 📋 Prerequisites

### Required Tools

1. **Visual Studio Build Tools 2022**
   - Download from: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
   - Install with "C++ build tools" workload
   - Or use: `winget install Microsoft.VisualStudio.2022.BuildTools`

2. **CMake 3.16+**
   - Download from: https://cmake.org/download/
   - Or use: `winget install Kitware.CMake`

3. **Git** (optional but recommended)
   - Download from: https://git-scm.com/download/win
   - Or use: `winget install Git.Git`

### Optional Tools

- **Python 3.11+** - For additional scripts and tools
- **Node.js** - For web-based tools and utilities

## 🔧 Installation Methods

### Method 1: Using winget (Windows Package Manager)

If you have winget installed (Windows 10 1709+ or Windows 11):

```cmd
# Install all tools at once
winget install Kitware.CMake
winget install Microsoft.VisualStudio.2022.BuildTools
winget install Git.Git
winget install Python.Python.3.11
winget install OpenJS.NodeJS
```

### Method 2: Using Chocolatey

If you have Chocolatey installed:

```cmd
# Install all tools at once
choco install cmake visualstudio2022buildtools git python nodejs -y
```

### Method 3: Manual Downloads

1. **Visual Studio Build Tools 2022**
   - Go to: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
   - Download "Build Tools for Visual Studio 2022"
   - Run installer and select "C++ build tools" workload

2. **CMake**
   - Go to: https://cmake.org/download/
   - Download Windows x64 installer
   - Install and add to PATH

3. **Git**
   - Go to: https://git-scm.com/download/win
   - Download and install with default settings

## 🏗️ Building the Project

### Step 1: Verify Tools Installation

Check if all tools are properly installed:

```cmd
# Check CMake
cmake --version

# Check Visual Studio Build Tools
where cl

# Check Git (optional)
git --version
```

### Step 2: Build the Project

#### Using the Enhanced Build Script (Recommended)

```cmd
# Build in Release mode
build.bat

# Build in Debug mode
build.bat --debug

# Clean build
build.bat --clean

# Build with tests
build.bat --test

# Build with verbose output
build.bat --verbose

# Auto-install missing tools
build.bat --auto-install
```

#### Manual CMake Build

```cmd
# Create build directory
mkdir build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Run tests (optional)
ctest --output-on-failure
```

### Step 3: Verify Build

After successful build, you should find:

```
build/bin/Release/uevrLCX.exe
```

## 🐛 Troubleshooting

### Common Issues

#### 1. "Visual Studio compiler not found"

**Solution:**
- Run `build.bat --auto-install` to automatically install Visual Studio Build Tools
- Or manually install from: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
- Make sure to select "C++ build tools" workload during installation

#### 2. "CMake is not installed"

**Solution:**
- Run `build.bat --auto-install` to automatically install CMake
- Or manually install from: https://cmake.org/download/
- Make sure CMake is added to your PATH

#### 3. Build fails with compiler errors

**Solutions:**
- Make sure you have the latest Visual Studio Build Tools
- Try running from a Visual Studio Developer Command Prompt
- Check that you have sufficient disk space (at least 5GB free)
- Ensure you have at least 8GB RAM

#### 4. PowerShell execution policy errors

**Solution:**
- Use `install_tools_wrapper.bat` instead of running PowerShell scripts directly
- Or run PowerShell as Administrator and execute: `Set-ExecutionPolicy RemoteSigned`

#### 5. Permission denied errors

**Solution:**
- Run the installer as Administrator
- Make sure you have write permissions to the project directory

### Advanced Troubleshooting

#### Check Visual Studio Installation

```cmd
# Find Visual Studio installations
vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64

# Check if vcvars64.bat exists
dir "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
```

#### Check CMake Configuration

```cmd
# Test CMake configuration
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON
```

#### Check Environment Variables

```cmd
# Check PATH for tools
where cmake
where cl
where git

# Check Visual Studio environment
echo %VS2022INSTALLDIR%
echo %VCINSTALLDIR%
```

## 🔄 Updating Tools

### Update CMake

```cmd
winget upgrade Kitware.CMake
```

### Update Visual Studio Build Tools

```cmd
winget upgrade Microsoft.VisualStudio.2022.BuildTools
```

### Update All Tools

```cmd
winget upgrade --all
```

## 📦 Package Managers

### Installing winget

If you don't have winget:

1. **Windows 10 1709+ or Windows 11**: winget should be pre-installed
2. **Older Windows versions**: Download from https://github.com/microsoft/winget-cli/releases

### Installing Chocolatey

Run PowerShell as Administrator and execute:

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

## 🎯 Next Steps

After successful build:

1. **Test the installation:**
   ```cmd
   build/bin/Release/uevrLCX.exe --help
   ```

2. **Run system diagnostics:**
   ```cmd
   build/bin/Release/uevrLCX.exe --test
   ```

3. **Try VR conversion:**
   ```cmd
   build/bin/Release/uevrLCX.exe --analyze "path/to/your/game.exe"
   ```

## 📞 Support

If you encounter issues:

1. Check this troubleshooting guide
2. Run `build.bat --verbose` for detailed output
3. Check the build logs in the `build` directory
4. Open an issue on GitHub with:
   - Your Windows version
   - Error messages
   - Build logs
   - Steps to reproduce

## 🔗 Useful Links

- **Visual Studio Build Tools**: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
- **CMake**: https://cmake.org/download/
- **Git**: https://git-scm.com/download/win
- **winget**: https://github.com/microsoft/winget-cli
- **Chocolatey**: https://chocolatey.org/
- **PowerShell**: https://docs.microsoft.com/en-us/powershell/

---

**Happy VR Converting! 🎮🥽**
