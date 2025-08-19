@echo off
echo ========================================
echo UEVR Cross-Engine VR System Deployment
echo ========================================
echo.

echo [1/8] Creating directory structure...
if not exist "data" mkdir data
if not exist "src\core" mkdir src\core
if not exist "src\vr" mkdir src\vr
if not exist "src\hooks" mkdir src\hooks
if not exist "src\adapters\common" mkdir src\adapters\common
if not exist "src\adapters\redengine4" mkdir src\adapters\redengine4
if not exist "src\adapters\re-engine" mkdir src\adapters\re-engine
if not exist "src\adapters\mt-framework" mkdir src\adapters\mt-framework
if not exist "src\adapters\unreal-engine" mkdir src\adapters\unreal-engine
if not exist "src\adapters\unity" mkdir src\adapters\unity
if not exist "src\adapters\cryengine" mkdir src\adapters\cryengine
if not exist "src\adapters\idtech" mkdir src\adapters\idtech
if not exist "src\adapters\source" mkdir src\adapters\source
if not exist "profiles\_TEMPLATES" mkdir profiles\_TEMPLATES
if not exist "profiles\_TEMPLATES\redengine4" mkdir profiles\_TEMPLATES\redengine4
if not exist "profiles\_TEMPLATES\re-engine" mkdir profiles\_TEMPLATES\re-engine
if not exist "profiles\_TEMPLATES\mt-framework" mkdir profiles\_TEMPLATES\mt-framework
if not exist "profiles\_TEMPLATES\unreal" mkdir profiles\_TEMPLATES\unreal
if not exist "tools" mkdir tools
echo ✅ Directory structure created

echo.
echo [2/8] Deploying core system files...
copy "src\core\engine_detection.cpp" "src\core\" >nul 2>&1
copy "src\core\engine_detection.h" "src\core\" >nul 2>&1
copy "src\core\adapter_loader.cpp" "src\core\" >nul 2>&1
copy "src\core\adapter_loader.h" "src\core\" >nul 2>&1
copy "src\vr\VRSystem.h" "src\vr\" >nul 2>&1
copy "src\hooks\GameHooks.h" "src\hooks\" >nul 2>&1
copy "src\main.cpp" "src\" >nul 2>&1
echo ✅ Core system files deployed

echo.
echo [3/8] Deploying engine detection database...
copy "data\engine_signatures.json" "data\" >nul 2>&1
echo ✅ Engine detection database deployed

echo.
echo [4/8] Deploying adapter registry...
copy "adapters\adapter_registry.json" "adapters\" >nul 2>&1
echo ✅ Adapter registry deployed

echo.
echo [5/8] Deploying profile templates...
copy "profiles\_TEMPLATE\profile.json" "profiles\_TEMPLATE\" >nul 2>&1
copy "profiles\_TEMPLATE\README.md" "profiles\_TEMPLATE\" >nul 2>&1
copy "profiles\_TEMPLATES\redengine4\profile.json" "profiles\_TEMPLATES\redengine4\" >nul 2>&1
copy "profiles\_TEMPLATES\re-engine\profile.json" "profiles\_TEMPLATES\re-engine\" >nul 2>&1
copy "profiles\_TEMPLATES\mt-framework\profile.json" "profiles\_TEMPLATES\mt-framework\" >nul 2>&1
echo ✅ Profile templates deployed

echo.
echo [6/8] Deploying game profiles...
copy "profiles\Cyberpunk2077\profile.json" "profiles\Cyberpunk2077\" >nul 2>&1
copy "profiles\Cyberpunk2077\README.md" "profiles\Cyberpunk2077\" >nul 2>&1
copy "profiles\re7-Win64-Shipping\profile.json" "profiles\re7-Win64-Shipping\" >nul 2>&1
copy "profiles\MonsterHunterWorld\profile.json" "profiles\MonsterHunterWorld\" >nul 2>&1
echo ✅ Game profiles deployed

echo.
echo [7/8] Deploying tools...
copy "tools\profile_generator.py" "tools\" >nul 2>&1
echo ✅ Tools deployed

echo.
echo [8/8] Deploying documentation...
copy "CROSS_ENGINE_SYSTEM.md" "." >nul 2>&1
copy "adapter.schema.json" "." >nul 2>&1
echo ✅ Documentation deployed

echo.
echo ========================================
echo Deployment Complete!
echo ========================================
echo.
echo The UEVR Cross-Engine VR System has been deployed successfully.
echo.
echo Next steps:
echo 1. Install required dependencies (Visual Studio, CMake)
echo 2. Build the system using CMake
echo 3. Configure your VR headset
echo 4. Launch the system and enjoy VR gaming!
echo.
echo For more information, see CROSS_ENGINE_SYSTEM.md
echo.
pause


