# UEVR Merge All Analyses
# Samler alle individuelle analyser til én samlet fil

Write-Host "=== SAMLER ALLE ANALYSER ===" -ForegroundColor Cyan

# Skift til parent directory (root af projektet)
Set-Location (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent)

# Opret docs/deepwiki/analyses mappe hvis den ikke findes
$analysesPath = "docs\deepwiki\analyses"
if (-not (Test-Path $analysesPath)) {
    New-Item -ItemType Directory -Path $analysesPath -Force | Out-Null
    Write-Host "OK Oprettet mappe: $analysesPath" -ForegroundColor Green
}

$outputFile = "docs\deepwiki\ALLE_ANALYSER_SAMLET.md"

$markdown = @"
# 🌟 UEVR Alle Analyser Samlet
## Komplet oversigt over alle system analyser

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Status:** 📚 Samlet fra alle analyser

---

## 📋 Indhold

Dette dokument indeholder links til alle individuelle analyser:

- [🔧 Core Files Analysis](analyses/CORE_ANALYSE.md)
- [🪝 Hooks Files Analysis](analyses/HOOKS_ANALYSE.md)
- [🎮 Mods Files Analysis](analyses/MODS_ANALYSE.md)
- [🔌 Adapters Files Analysis](analyses/ADAPTERS_ANALYSE.md)
- [🥽 VR Files Analysis](analyses/VR_ANALYSE.md)
- [🎨 ImGui Files Analysis](analyses/IMGUI_ANALYSE.md)
- [🛠️ Utility Files Analysis](analyses/UTILITY_ANALYSE.md)
- [❌ Missing Functions Analysis](analyses/MANGLENDE_FUNKTIONER.md)

---

## 🎯 Formål

Hver analyse giver detaljeret indsigt i en specifik del af UEVR systemet:

### 🔧 **Core System**
- Framework og hovedsystem
- Mod og plugin system
- Window filtering

### 🪝 **Hook System**
- Graphics API hooks (D3D11, D3D12, OpenGL, Vulkan)
- Input hooks (DInput, XInput)
- Window message hooks

### 🎮 **Mods System**
- Plugin loader
- Lua integration
- VR mods

### 🔌 **Adapters System**
- Engine-specifikke implementeringer
- REDengine 4 support
- UObject hooking

### 🥽 **VR System**
- OpenXR integration
- OpenVR support
- VR rendering

### 🎨 **ImGui System**
- UI management
- Theme helpers
- Window system

### 🛠️ **Utility System**
- Configuration management
- File utilities
- Logging system

---

## 📊 Samlet Status

Alle analyser er genereret og gemt i `docs/deepwiki/` mappen.

---

## 🚀 Næste Skridt

1. **Læs individuelle analyser** for at forstå hver del af systemet
2. **Identificer manglende implementeringer** fra missing functions analysen
3. **Implementer manglende funktioner** systematisk
4. **Kør analyserne igen** for at se fremskridt

---

*UEVR DeepWiki - Den dybe tallerken for alle projekters fulde filer* 🍕
"@

$markdown | Out-File -FilePath $outputFile -Encoding UTF8
Write-Host "✅ Alle analyser samlet i: $outputFile" -ForegroundColor Cyan
