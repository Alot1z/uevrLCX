# 🚀 SMART FIL LINKING SYSTEM
# Håndterer store filer ved at oprette kondenserede versioner og smarte links

Write-Host "🔗 Opretter Smart Fil Linking System..." -ForegroundColor Green

# Opret output mapper
$outputDir = "docs\deepwiki"
$smartLinksDir = "docs\smart_links"
if (!(Test-Path $outputDir)) { New-Item -ItemType Directory -Path $outputDir -Force }
if (!(Test-Path $smartLinksDir)) { New-Item -ItemType Directory -Path $smartLinksDir -Force }

# Funktion til at oprette smarte fil links
function Create-SmartFileLink {
    param(
        [string]$FilePath,
        [string]$OutputDir
    )
    
    if (!(Test-Path $FilePath)) {
        Write-Host "⚠️ Fil ikke fundet: $FilePath" -ForegroundColor Yellow
        return $null
    }
    
    try {
        $content = Get-Content $FilePath -Raw -Encoding UTF8
        $lines = $content.Split("`n")
        $fileName = Split-Path $FilePath -Leaf
        $fileBaseName = Split-Path $FilePath -LeafBase
        
        Write-Host "📁 Opretter smart link for: $fileName ($($lines.Count) linjer)" -ForegroundColor Yellow
        
        # Opret kondenseret version (første 100 + sidste 100 linjer)
        $condensedContent = @()
        $condensedContent += "# 📄 $fileName - KONDENSERET VERSION"
        $condensedContent += "## 📋 **FIL OVERSIGT**"
        $condensedContent += "- **Total Linjer**: $($lines.Count)"
        $condensedContent += "- **Fil Sti**: $FilePath"
        $condensedContent += "- **Genereret**: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")"
        $condensedContent += ""
        
        # Første 100 linjer
        $condensedContent += "## 🔍 **FØRSTE 100 LINJER**"
        $condensedContent += "```cpp"
        for ($i = 0; $i -lt [Math]::Min(100, $lines.Count); $i++) {
            $condensedContent += "$($i + 1): $($lines[$i])"
        }
        $condensedContent += "```"
        $condensedContent += ""
        
        # Sidste 100 linjer
        if ($lines.Count -gt 100) {
            $condensedContent += "## 🔍 **SIDSTE 100 LINJER**"
            $condensedContent += "```cpp"
            $startLine = [Math]::Max(0, $lines.Count - 100)
            for ($i = $startLine; $i -lt $lines.Count; $i++) {
                $condensedContent += "$($i + 1): $($lines[$i])"
            }
            $condensedContent += "```"
            $condensedContent += ""
        }
        
        # Funktion oversigt
        $condensedContent += "## 🔧 **FUNKTION OVERSIGT**"
        $functions = @()
        for ($i = 0; $i -lt $lines.Count; $i++) {
            $line = $lines[$i].Trim()
            if ($line -match "^\s*(\w+\s+)?(\w+)\s+(\w+)\s*\([^)]*\)\s*\{?\s*$") {
                $functions += "Linje $($i + 1): $($matches[2]) $($matches[3])"
            }
        }
        
        if ($functions.Count -gt 0) {
            $condensedContent += "**Funktioner fundet:**"
            foreach ($func in $functions) {
                $condensedContent += "- $func"
            }
        } else {
            $condensedContent += "Ingen funktioner fundet i denne fil."
        }
        
        $condensedContent += ""
        
        # Smart links til cursor chat
        $condensedContent += "## 🔗 **SMART LINKS TIL CURSOR CHAT**"
        $condensedContent += "### **📋 Komplet fil indhold:**"
        $condensedContent += "```bash"
        $condensedContent += "# Brug denne kommando for at få komplet fil indhold:"
        $condensedContent += "Get-Content `"$FilePath`" -Raw | Out-String"
        $condensedContent += "```"
        $condensedContent += ""
        
        $condensedContent += "### **🔍 Funktion analyse:**"
        $condensedContent += "```bash"
        $condensedContent += "# Analyser alle funktioner i filen:"
        $condensedContent += "Select-String -Path `"$FilePath`" -Pattern `"^\s*\w+\s+\w+\s*\([^)]*\)\s*\{?\s*$`""
        $condensedContent += "```"
        $condensedContent += ""
        
        $condensedContent += "### **📊 Fil statistik:**"
        $condensedContent += "```bash"
        $condensedContent += "# Få fil statistik:"
        $condensedContent += "Get-ChildItem `"$FilePath`" | Select-Object Name, Length, LastWriteTime"
        $condensedContent += "```"
        $condensedContent += ""
        
        # Context7 + DeepWiki integration
        $condensedContent += "## 🌐 **CONTEXT7 + DEEPWIKI INTEGRATION**"
        $condensedContent += "### **Context7 kommandoer:**"
        $condensedContent += "```bash"
        $condensedContent += "# Analyser fil med Context7:"
        $condensedContent += "mcp_context7_resolve-library-id --libraryName `"$fileBaseName`""
        $condensedContent += "mcp_context7_get-library-docs --context7CompatibleLibraryID `/praydog/uevr` --topic `"$fileBaseName`""
        $condensedContent += "```"
        $condensedContent += ""
        
        $condensedContent += "### **DeepWiki kommandoer:**"
        $condensedContent += "```bash"
        $condensedContent += "# Analyser fil med DeepWiki:"
        $condensedContent += "mcp_deepwiki_ask_question --repoName `"praydog/uevr` --question `"How is $fileBaseName implemented?`""
        $condensedContent += "mcp_deepwiki_ask_question --repoName `"praydog/uevr` --question `"What does $fileBaseName do?`""
        $condensedContent += "```"
        $condensedContent += ""
        
        # Gem kondenseret version
        $condensedFile = Join-Path $OutputDir "$fileBaseName`_KONDENSERET.md"
        $condensedContent -join "`n" | Out-File $condensedFile -Encoding UTF8
        
        # Opret smart link fil
        $smartLinkFile = Join-Path $smartLinksDir "$fileBaseName`_SMART_LINK.md"
        $smartLinkContent = @"
# 🔗 SMART LINK: $fileName

## 📋 **FIL INFORMATION**
- **Filnavn**: $fileName
- **Sti**: $FilePath
- **Total Linjer**: $($lines.Count)
- **Status**: Kondenseret version oprettet

## 🚀 **SÅDAN BRUGER DU DETTE I CURSOR CHAT**

### **1. 📄 Få kondenseret version:**
```bash
# Læs kondenseret version
Get-Content "docs\deepwiki\${fileBaseName}_KONDENSERET.md" -Raw
```

### **2. 🔍 Analyser specifikke dele:**
```bash
# Få første 50 linjer
Get-Content "$FilePath" | Select-Object -First 50

# Få sidste 50 linjer  
Get-Content "$FilePath" | Select-Object -Last 50

# Find alle funktioner
Select-String -Path "$FilePath" -Pattern "^\s*\w+\s+\w+\s*\([^)]*\)\s*\{?\s*$"
```

### **3. 🌐 Brug Context7 + DeepWiki:**
```bash
# Context7 på filen
mcp_context7_resolve-library-id --libraryName "$fileBaseName"

# DeepWiki på filen
mcp_deepwiki_ask_question --repoName "praydog/uevr" --question "How is $fileBaseName implemented?"
```

### **4. 📊 Få fil statistik:**
```bash
# Fil info
Get-ChildItem "$FilePath" | Select-Object Name, Length, LastWriteTime

# Linje tælling
(Get-Content "$FilePath").Count
```

## 🎯 **FORDELE VED DETTE SYSTEM**

✅ **Håndterer store filer** - Ingen problemer med 1000+ linjer  
✅ **Kondenseret oversigt** - Første + sidste linjer  
✅ **Smart links** - Nem adgang til alle dele  
✅ **Context7 integration** - Autoritativ viden  
✅ **DeepWiki integration** - Repository-specifik viden  
✅ **Cursor chat venlig** - Alle kommandoer klar til brug  

## 📁 **OPRETTEDE FILER**

- **Kondenseret version**: `docs\deepwiki\${fileBaseName}_KONDENSERET.md`
- **Smart link**: `docs\smart_links\${fileBaseName}_SMART_LINK.md`
- **Original fil**: `$FilePath`

---

**🚀 Nu kan du nemt linke store filer til cursor chatten!**
"@
        
        $smartLinkContent | Out-File $smartLinkFile -Encoding UTF8
        
        Write-Host "✅ Smart link oprettet for $fileName" -ForegroundColor Green
        Write-Host "   - Kondenseret: $condensedFile" -ForegroundColor Cyan
        Write-Host "   - Smart link: $smartLinkFile" -ForegroundColor Cyan
        
        return @{
            FileName = $fileName
            FilePath = $FilePath
            TotalLines = $lines.Count
            CondensedFile = $condensedFile
            SmartLinkFile = $smartLinkFile
        }
        
    } catch {
        Write-Host "❌ Fejl ved oprettelse af smart link for $FilePath : $($_.Exception.Message)" -ForegroundColor Red
        return $null
    }
}

# Opret smart links for alle store filer
$largeFiles = @(
    "src/Framework.cpp",
    "src/Framework.hpp",
    "src/Plugin.cpp",
    "src/Plugin.hpp",
    "src/Main.cpp",
    "project.json",
    "files.md"
)

$smartLinks = @()

foreach ($file in $largeFiles) {
    $smartLink = Create-SmartFileLink -FilePath $file -OutputDir $outputDir
    if ($smartLink) {
        $smartLinks += $smartLink
    }
}

# Opret samlet smart linking rapport
$smartLinkReport = @"
# 🔗 SMART FIL LINKING SYSTEM - KOMPLET RAPPORT

## 📋 **SYSTEM OVERSIGT**
- **Total Filer**: $($smartLinks.Count)
- **Genereret**: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
- **Status**: Alle smart links oprettet

---

## 📁 **OPRETTEDE SMART LINKS**

"@

foreach ($link in $smartLinks) {
    $smartLinkReport += @"

### **📄 $($link.FileName)**
- **Sti**: $($link.FilePath)
- **Total Linjer**: $($link.TotalLines)
- **Kondenseret**: $($link.CondensedFile)
- **Smart Link**: $($link.SmartLinkFile)

**Sådan bruger du det i cursor chat:**
```bash
# Læs kondenseret version
Get-Content "$($link.CondensedFile)" -Raw

# Analyser original fil
Get-Content "$($link.FilePath)" | Select-Object -First 100

# Brug Context7
mcp_context7_resolve-library-id --libraryName "$(Split-Path $link.FilePath -LeafBase)"

# Brug DeepWiki
mcp_deepwiki_ask_question --repoName "praydog/uevr" --question "How is $(Split-Path $link.FilePath -LeafBase) implemented?"
```

"@
}

$smartLinkReport += @"

---

## 🚀 **SÅDAN BRUGER DU DETTE SYSTEM**

### **1. 📄 Få kondenseret oversigt:**
Alle store filer har nu kondenserede versioner i `docs\deepwiki\` mappen.

### **2. 🔗 Brug smart links:**
Alle smart links er gemt i `docs\smart_links\` mappen med komplette instruktioner.

### **3. 🌐 Integrer med Context7 + DeepWiki:**
Hver fil har nu Context7 og DeepWiki kommandoer klar til brug.

### **4. 💬 Brug i cursor chat:**
Alle kommandoer er formateret til nem brug i cursor chatten.

---

## 🎯 **FORDELE**

✅ **Håndterer store filer** - Ingen problemer med 1000+ linjer  
✅ **Kondenseret oversigt** - Første + sidste linjer  
✅ **Smart links** - Nem adgang til alle dele  
✅ **Context7 integration** - Autoritativ viden  
✅ **DeepWiki integration** - Repository-specifik viden  
✅ **Cursor chat venlig** - Alle kommandoer klar til brug  

---

**🚀 Nu kan du nemt linke store filer til cursor chatten!**
"@

# Gem samlet rapport
$reportFile = Join-Path $outputDir "SMART_FIL_LINKING_RAPPORT.md"
$smartLinkReport | Out-File $reportFile -Encoding UTF8

Write-Host "📊 Smart fil linking system færdig!" -ForegroundColor Green
Write-Host "   - Smart links oprettet: $($smartLinks.Count)" -ForegroundColor Cyan
Write-Host "   - Kondenserede filer: $($smartLinks.Count)" -ForegroundColor Cyan
Write-Host "   - Samlet rapport: $reportFile" -ForegroundColor Cyan

exit 0
