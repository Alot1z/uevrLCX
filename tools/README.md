# uevr Cross-Engine VR Adapters - Build Tools

## 🏗️ **STRUKTURERET BUILD SYSTEM**

Dette build system er organiseret i 4 separate mapper for hver komponent, hvilket giver bedre organisering og lettere vedligeholdelse.

## 📁 **MAPPE STRUKTUR**

```
tools/
├── build_main_cross_engine/          # Hoved Cross-Engine Framework
│   ├── build_main_framework.ps1     # PowerShell build script
│   └── build_main_framework.bat     # Batch wrapper
│
├── build_mt_framework/               # Monster Hunter World Adapter
│   ├── build_mt_framework.ps1       # PowerShell build script
│   └── build_mt_framework.bat       # Batch wrapper
│
├── build_re_engine/                  # Resident Evil 7 Adapter
│   ├── build_re_engine.ps1          # PowerShell build script
│   └── build_re_engine.bat          # Batch wrapper
│
├── build_redengine4/                 # Cyberpunk 2077 Adapter
│   ├── build_redengine4.ps1         # PowerShell build script
│   └── build_redengine4.bat         # Batch wrapper
│
└── build_all_cross_engine/           # Master Build System
    ├── build_all_components.ps1     # PowerShell master script
    └── build_all_components.bat     # Batch wrapper
```

## 🎯 **KOMPONENTER**

### **1. Main Cross-Engine Framework**
- **Mål:** Bygger hoved cross-engine systemet og registry
- **Output:** `uevr_cross_engine_system.dll`
- **Lokation:** `SOURCECODE/uevr/build/`
- **Script:** `tools/build_main_cross_engine/build_main_framework.bat`

### **2. MT Framework Adapter**
- **Mål:** Monster Hunter World VR adapter
- **Output:** `mt_framework_cross_engine_adapter.dll`
- **Deployment:** `profiles/MonsterHunterWorld/`
- **Script:** `tools/build_mt_framework/build_mt_framework.bat`

### **3. RE Engine Adapter**
- **Mål:** Resident Evil 7 VR adapter
- **Output:** `re_engine_adapter.dll`
- **Deployment:** `profiles/re7/`
- **Script:** `tools/build_re_engine/build_re_engine.bat`

### **4. REDengine 4 Adapter**
- **Mål:** Cyberpunk 2077 VR adapter
- **Output:** `redengine4_adapter.dll`
- **Deployment:** `profiles/Cyberpunk2077/`
- **Script:** `tools/build_redengine4/build_redengine4.bat`

## 🚀 **BRUG AF BUILD SYSTEMET**

### **Individuelle Komponenter**

#### **Byg Main Framework:**
```bash
cd tools/build_main_cross_engine
build_main_framework.bat
```

#### **Byg MT Framework Adapter:**
```bash
cd tools/build_mt_framework
build_mt_framework.bat
```

#### **Byg RE Engine Adapter:**
```bash
cd tools/build_re_engine
build_re_engine.bat
```

#### **Byg REDengine 4 Adapter:**
```bash
cd tools/build_redengine4
build_redengine4.bat
```

### **Byg Alle Komponenter**

#### **Master Build Script:**
```bash
cd tools/build_all_cross_engine
build_all_components.bat
```

Dette script vil:
1. Bygge alle 4 komponenter sekventielt
2. Håndtere fejl og fortsætte med næste komponent
3. Deploye alle adapters til deres respektive profile mapper
4. Generere detaljerede logs for hver komponent

## ⚙️ **BUILD KONFIGURATION**

### **Standard Indstillinger**
- **Build Type:** Release
- **Warnings as Errors:** Enabled (`/W4 /WX`)
- **Parallel Jobs:** 8
- **Compiler:** Visual Studio 2022
- **Architecture:** x64

### **Tilpasning**
Hver PowerShell script accepterer parametre:
```powershell
param(
    [string]$BuildType = "Release",           # Debug/Release
    [switch]$WarningsAsErrors = $true,       # Enable/disable
    [int]$ParallelJobs = 8                   # Number of parallel jobs
)
```

## 📊 **LOGGING OG VALIDERING**

### **Log Struktur**
```
logs/build/
├── main_cross_engine/           # Main framework logs
├── mt_framework/                # MT Framework logs
├── re_engine/                   # RE Engine logs
├── redengine4/                  # REDengine 4 logs
└── all_components/              # Master build logs
```

### **Log Filer**
Hver komponent genererer:
- `build.log` - Komplet build log
- `cmake_config.log` - CMake konfiguration
- `cmake_build.log` - CMake build output
- `error.log` - Fejl detaljer
- `build_success.txt` / `build_failed.txt` - Status markører

### **Build Validering**
Hver build script validerer:
- CMake konfiguration
- Build output filer
- Deployment til profile mapper
- Fejl håndtering og recovery

## 🔧 **AVANCEREDE FUNKTIONER**

### **Skip Komponenter**
Master build script tillader at springe specifikke komponenter over:
```bash
# Spring Main Framework over
build_all_components.bat -SkipMainFramework

# Spring kun MT Framework over
build_all_components.bat -SkipMTFramework

# Spring flere over
build_all_components.bat -SkipMainFramework -SkipREDengine4
```

### **Fejl Håndtering**
- **Graceful Degradation:** Hvis en komponent fejler, fortsætter scriptet med næste
- **Detaljerede Fejl Logs:** Alle fejl logges med stack traces
- **Build Status Tracking:** Hver komponent får en status markør
- **Recovery Information:** Scriptet giver vejledning til fejl løsning

### **Performance Optimering**
- **Parallel Compilation:** 8 parallel build jobs som standard
- **Incremental Builds:** CMake håndterer automatisk incremental builds
- **Dependency Caching:** CMake cacher dependency information
- **Build Time Tracking:** Måler og rapporterer build tid for hver komponent

## 🚨 **FEJL LØSNING**

### **Almindelige Problemer**

#### **CMake Ikke Fundet**
```bash
ERROR: CMake not found in PATH
```
**Løsning:** Installer CMake 3.16+ og tilføj til PATH

#### **Visual Studio Ikke Fundet**
```bash
ERROR: Visual Studio compiler not found
```
**Løsning:** Installer Visual Studio 2019/2022 med C++ workload

#### **PowerShell Execution Policy**
```bash
ERROR: Execution policy prevents running scripts
```
**Løsning:** Scriptet bruger automatisk `-ExecutionPolicy Bypass`

#### **Build Fejl med Warnings-as-Errors**
```bash
ERROR: Build failed due to warnings
```
**Løsning:** Fix alle warnings eller midlertidigt deaktiver `/WX` flag

### **Debug Information**
- **Logs:** Alle build scripts genererer detaljerede logs
- **Error Files:** Separate error log filer for hver komponent
- **Status Markers:** Success/failure markører i log mapper
- **Build Output:** CMake output gemmes for analyse

## 📋 **KRÆVEDE PREREQUISITES**

### **Software**
- **CMake 3.16+** - Build system generator
- **Visual Studio 2019/2022** - C++ compiler og IDE
- **PowerShell 5.1+** - Script execution engine
- **Git** - Version control (for dependencies)

### **Dependencies**
- **MinHook** - Function hooking library
- **spdlog** - Logging library
- **nlohmann/json** - JSON parsing library
- **DirectX SDK** - Graphics API support

## 🎯 **NÆSTE TRIN**

Efter succesfuld build:

1. **Integration Testing:** Test cross-engine funktionalitet
2. **Performance Validation:** Verificer VR performance targets
3. **User Testing:** Valider VR oplevelse kvalitet
4. **Community Deployment:** Release til bruger community

## 📞 **SUPPORT**

### **Build Problemer**
- Tjek logs i `logs/build/[component]/`
- Verificer prerequisites er installeret
- Kør individuelle build scripts for isolation

### **Integration Problemer**
- Verificer alle adapters er deployed til profile mapper
- Tjek cross-engine registry integration
- Valider factory pattern implementation

---

**Build System Version:** 1.0.0  
**Status:** Production Ready  
**Næste Phase:** Integration Testing og Community Deployment
