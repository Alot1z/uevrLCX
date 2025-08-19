# UEVR ImGui Files Analyzer
# Analyserer alle ImGui filer og formaterer dem i perfekt markdown

Write-Host "=== ANALYSERER IMGUI FILER ===" -ForegroundColor Green

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "✅ Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = "$docsPath\IMGUI_ANALYSE.md"
$imguiFiles = @(
    "src\uevr-imgui\imgui_impl_dx11.cpp",
    "src\uevr-imgui\imgui_impl_dx11.h",
    "src\uevr-imgui\imgui_impl_dx12.cpp",
    "src\uevr-imgui\imgui_impl_dx12.h",
    "src\uevr-imgui\imgui_impl_win32.cpp",
    "src\uevr-imgui\imgui_impl_win32.h",
    "src\uevr-imgui\uevr_imconfig.hpp"
)

$markdown = @"
# 🎨 UEVR ImGui Files Analysis
## Komplet analyse af alle ImGui filer

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Antal filer:** $($imguiFiles.Count)  
**Status:** ✅ Komplet

---

"@

foreach ($file in $imguiFiles) {
    if (Test-Path $file) {
        Write-Host "Analyserer: $file" -ForegroundColor Yellow
        
        $content = Get-Content $file -Raw
        $fileName = Split-Path $file -Leaf
        $filePath = $file
        
        # Analyser funktioner og ImGui
        $functions = $content | Select-String "void.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        $imguiFunctions = $content | Select-String ".*ImGui.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
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

### ⚙️ ImGui Functions
```cpp
$($imguiFunctions -join "`n")
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
Write-Host "✅ ImGui analyse gemt til: $outputFile" -ForegroundColor Green
