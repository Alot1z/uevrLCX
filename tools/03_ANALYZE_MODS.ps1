# UEVR Mods Files Analyzer
# Analyserer alle mod filer og formaterer dem i perfekt markdown

Write-Host "=== ANALYSERER MODS FILER ===" -ForegroundColor Green

# Opret docs/deepwiki/analyses mappe hvis den ikke findes
$analysesPath = "docs\deepwiki\analyses"
if (-not (Test-Path $analysesPath)) {
    New-Item -ItemType Directory -Path $analysesPath -Force | Out-Null
    Write-Host "OK Oprettet mappe: $analysesPath" -ForegroundColor Green
}

$outputFile = "$analysesPath\MODS_ANALYSE.md"
$modFiles = @(
    "src\mods\FrameworkConfig.cpp",
    "src\mods\FrameworkConfig.hpp",
    "src\mods\ImGuiThemeHelpers.cpp",
    "src\mods\ImGuiThemeHelpers.hpp",
    "src\mods\LuaLoader.cpp",
    "src\mods\LuaLoader.hpp",
    "src\mods\PluginLoader.cpp",
    "src\mods\PluginLoader.hpp",
    "src\mods\UObjectHook.cpp",
    "src\mods\UObjectHook.hpp",
    "src\mods\VR.cpp",
    "src\mods\VR.hpp"
)

$markdown = @"
# 🎮 UEVR Mods Files Analysis
## Komplet analyse af alle mod filer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Antal filer:** $($modFiles.Count)  
**Status:** ✅ Komplet

---

"@

foreach ($file in $modFiles) {
    if (Test-Path $file) {
        Write-Host "Analyserer: $file" -ForegroundColor Yellow
        
        $content = Get-Content $file -Raw
        $fileName = Split-Path $file -Leaf
        $filePath = $file
        
        # Analyser funktioner og mods
        $functions = $content | Select-String "void.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $modFunctions = $content | Select-String "on_.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $classes = $content | Select-String "class.*\{" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $includes = $content | Select-String "#include.*" | ForEach-Object { $_.Matches[0].Value.Trim() }
        
        $markdown += @"

## 📁 $fileName
**Sti:** `$filePath`  
**Størrelse:** $((Get-Item $file).Length) bytes  
**Sidst ændret:** $((Get-Item $file).LastWriteTime.ToString("yyyy-MM-dd HH:mm:ss"))

### 🔗 Includes
```cpp
$($includes -join "`n")
```

### 🏗️ Classes
```cpp
$($classes -join "`n")
```

### ⚙️ Mod Event Functions
```cpp
$($modFunctions -join "`n")
```

### ⚙️ General Functions
```cpp
$($functions -join "`n")
```

### 📄 Komplet Indhold
```cpp
$content
```

---
"@
    }
}

$markdown | Out-File -FilePath $outputFile -Encoding UTF8
Write-Host "✅ Mods analyse gemt til: $outputFile" -ForegroundColor Green
