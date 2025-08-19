# UEVR Hooks Files Analyzer
# Analyserer alle hook filer og formaterer dem i perfekt markdown

Write-Host "=== ANALYSERER HOOKS FILER ===" -ForegroundColor Green

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "✅ Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = "$docsPath\HOOKS_ANALYSE.md"
$hookFiles = @(
    "src\hooks\D3D11Hook.cpp",
    "src\hooks\D3D11Hook.hpp",
    "src\hooks\D3D12Hook.cpp", 
    "src\hooks\D3D12Hook.hpp",
    "src\hooks\DInputHook.cpp",
    "src\hooks\DInputHook.hpp",
    "src\hooks\DirectXHooks.cpp",
    "src\hooks\EngineHooks.cpp",
    "src\hooks\GameHooks.h",
    "src\hooks\HookManager.cpp",
    "src\hooks\OpenGLHooks.cpp",
    "src\hooks\VulkanHooks.cpp",
    "src\hooks\WindowsMessageHook.cpp",
    "src\hooks\WindowsMessageHook.hpp",
    "src\hooks\XInputHook.cpp",
    "src\hooks\XInputHook.hpp"
)

$markdown = @"
# 🪝 UEVR Hooks Files Analysis
## Komplet analyse af alle hook filer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Antal filer:** $($hookFiles.Count)  
**Status:** ✅ Komplet

---

"@

foreach ($file in $hookFiles) {
    if (Test-Path $file) {
        Write-Host "Analyserer: $file" -ForegroundColor Yellow
        
        $content = Get-Content $file -Raw
        $fileName = Split-Path $file -Leaf
        $filePath = $file
        
        # Analyser funktioner og hooks
        $functions = $content | Select-String "void.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $hookFunctions = $content | Select-String ".*Hook.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
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

### ⚙️ Hook Functions
```cpp
$($hookFunctions -join "`n")
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
Write-Host "✅ Hooks analyse gemt til: $outputFile" -ForegroundColor Green
