# UEVR Missing Functions Analyzer
# Analyserer alle manglende funktioner og opretter en komplet rapport

Write-Host "=== ANALYSERER MANGLENDE FUNKTIONER ===" -ForegroundColor Red

# Opret docs/deepwiki mappe hvis den ikke findes
$docsPath = "docs\deepwiki"
if (-not (Test-Path $docsPath)) {
    New-Item -ItemType Directory -Path $docsPath -Force | Out-Null
    Write-Host "✅ Oprettet mappe: $docsPath" -ForegroundColor Green
}

$outputFile = "$docsPath\MANGLENDE_FUNKTIONER.md"

$markdown = @"
# ❌ UEVR Missing Functions Analysis
## Komplet oversigt over alle manglende funktioner

**Genereret:** $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**Status:** ⚠️ KRITISK - Mange funktioner mangler implementering

---

## 📊 Oversigt over manglende funktioner

"@

# Analyser alle .hpp filer for manglende implementeringer
$headerFiles = Get-ChildItem -Path "src" -Recurse -Include "*.hpp", "*.h" | Where-Object { $_.FullName -notlike "*build*" -and $_.FullName -notlike "*deps*" }

$totalMissing = 0
$missingByFile = @{}

foreach ($headerFile in $headerFiles) {
    $headerContent = Get-Content $headerFile.FullName -Raw
    $headerFunctions = $headerContent | Select-String "void.*\(.*\);" | ForEach-Object { $_.Matches[0].Value.Trim() }
    
    # Find tilsvarende .cpp fil
    $cppFile = $headerFile.FullName -replace "\.hpp$", ".cpp" -replace "\.h$", ".cpp"
    
    if (Test-Path $cppFile) {
        $cppContent = Get-Content $cppFile -Raw
        $implementedFunctions = $cppContent | Select-String "void.*::.*\(.*\)" | ForEach-Object { $_.Matches[0].Value.Trim() }
        
        $missingFunctions = @()
        foreach ($headerFunc in $headerFunctions) {
            $funcName = $headerFunc -replace "void\s+", "" -replace "\(.*\)", ""
            $found = $false
            
            foreach ($implFunc in $implementedFunctions) {
                if ($implFunc -like "*$funcName*") {
                    $found = $true
                    break
                }
            }
            
            if (-not $found) {
                $missingFunctions += $headerFunc
                $totalMissing++
            }
        }
        
        if ($missingFunctions.Count -gt 0) {
            $missingByFile[$headerFile.Name] = $missingFunctions
        }
    }
}

# Sorter efter antal manglende funktioner
$sortedFiles = $missingByFile.GetEnumerator() | Sort-Object { $_.Value.Count } -Descending

foreach ($fileEntry in $sortedFiles) {
    $fileName = $fileEntry.Key
    $missingFuncs = $fileEntry.Value
    
    $markdown += @"

## 📁 $fileName
**Antal manglende funktioner:** $($missingFuncs.Count)

### ❌ Manglende implementeringer:
"@
    
    foreach ($func in $missingFuncs) {
        $markdown += @"
- `$func`
"@
    }
}

$markdown += @"

---

## 📈 Statistik

- **Total antal header filer:** $($headerFiles.Count)
- **Filer med manglende funktioner:** $($missingByFile.Count)
- **Total antal manglende funktioner:** $totalMissing

## 🚨 Prioriterede filer der skal implementeres

"@

# Top 5 filer med flest manglende funktioner
$top5 = $sortedFiles | Select-Object -First 5
for ($i = 0; $i -lt $top5.Count; $i++) {
    $entry = $top5[$i]
    $markdown += @"
$($i + 1). **$($entry.Key)** - $($entry.Value.Count) manglende funktioner
"@
}

$markdown += @"

---

## 💡 Anbefalinger

1. **Start med filer der har flest manglende funktioner**
2. **Implementer funktioner i rækkefølge af kompleksitet**
3. **Test hver implementering grundigt**
4. **Brug deepwiki og context7 til at forstå funktionaliteten**

---

*Denne rapport er automatisk genereret og skal opdateres løbende*
"@

$markdown | Out-File -FilePath $outputFile -Encoding UTF8
Write-Host "✅ Manglende funktioner analyse gemt til: $outputFile" -ForegroundColor Red
Write-Host "⚠️  Total manglende funktioner: $totalMissing" -ForegroundColor Red
