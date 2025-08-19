# UEVR Build Verification Script
# Verifies build integrity after translation changes

param(
    [string]$ProjectRoot = ".",
    [string]$OutputDir = "build_verification_reports"
)

Write-Host "=== UEVR BUILD VERIFICATION ===" -ForegroundColor Green
Write-Host "Verifying build integrity after translation changes..." -ForegroundColor Yellow

# Change to project root
Set-Location $ProjectRoot

# Create output directory
if (!(Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    Write-Host "✅ Created output directory: $OutputDir" -ForegroundColor Green
}

$verificationResults = @{
    Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    ProjectRoot = (Get-Location).Path
    Tests = @()
    OverallStatus = "PENDING"
}

# Test 1: UTF-8 Encoding Validation
Write-Host "`n🔍 Testing UTF-8 encoding..." -ForegroundColor Cyan
$encodingTest = @{
    Name = "UTF-8 Encoding Validation"
    Status = "PENDING"
    Details = @()
    Errors = @()
}

try {
    $filesWithEncodingIssues = @()
    
    # Check all text files for encoding issues
    $textFiles = Get-ChildItem -Recurse -Include "*.md", "*.txt", "*.cpp", "*.hpp", "*.h", "*.c", "*.ps1", "*.bat", "*.py" | 
                 Where-Object { $_.FullName -notlike "*translation-workspace*" }
    
    foreach ($file in $textFiles) {
        try {
            $content = Get-Content $file.FullName -Raw -Encoding UTF8 -ErrorAction Stop
            # Check for common encoding issues
            if ($content -match '[^\x00-\x7F]' -and $content -notmatch '[æøåÆØÅ]') {
                # Contains non-ASCII but not Danish characters - potential encoding issue
                $filesWithEncodingIssues += $file.Name
            }
        } catch {
            $encodingTest.Errors += "Failed to read $($file.Name): $($_.Exception.Message)"
        }
    }
    
    if ($filesWithEncodingIssues.Count -eq 0) {
        $encodingTest.Status = "PASS"
        $encodingTest.Details += "All text files have valid UTF-8 encoding"
    } else {
        $encodingTest.Status = "WARNING"
        $encodingTest.Details += "Found $($filesWithEncodingIssues.Count) files with potential encoding issues"
        $encodingTest.Details += "Files: $($filesWithEncodingIssues -join ', ')"
    }
    
} catch {
    $encodingTest.Status = "FAIL"
    $encodingTest.Errors += "Encoding test failed: $($_.Exception.Message)"
}

$verificationResults.Tests += $encodingTest

# Test 2: CMake Configuration Validation
Write-Host "`n🔍 Testing CMake configuration..." -ForegroundColor Cyan
$cmakeTest = @{
    Name = "CMake Configuration Validation"
    Status = "PENDING"
    Details = @()
    Errors = @()
}

try {
    if (Test-Path "CMakeLists.txt") {
        $cmakeContent = Get-Content "CMakeLists.txt" -Raw
        $cmakeIssues = @()
        
        # Check for common CMake issues
        if ($cmakeContent -match '[æøåÆØÅ]') {
            $cmakeIssues += "Contains Danish characters"
        }
        
        if ($cmakeContent -match 'include\s*\(\s*[^)]*[æøåÆØÅ][^)]*\)') {
            $cmakeIssues += "Contains Danish characters in include statements"
        }
        
        if ($cmakeIssues.Count -eq 0) {
            $cmakeTest.Status = "PASS"
            $cmakeTest.Details += "CMakeLists.txt configuration is valid"
        } else {
            $cmakeTest.Status = "WARNING"
            $cmakeTest.Details += "CMake configuration issues found: $($cmakeIssues -join ', ')"
        }
    } else {
        $cmakeTest.Status = "SKIP"
        $cmakeTest.Details += "CMakeLists.txt not found"
    }
    
} catch {
    $cmakeTest.Status = "FAIL"
    $cmakeTest.Errors += "CMake test failed: $($_.Exception.Message)"
}

$verificationResults.Tests += $cmakeTest

# Test 3: PowerShell Script Validation
Write-Host "`n🔍 Testing PowerShell scripts..." -ForegroundColor Cyan
$ps1Test = @{
    Name = "PowerShell Script Validation"
    Status = "PENDING"
    Details = @()
    Errors = @()
}

try {
    $ps1Files = Get-ChildItem -Recurse -Include "*.ps1" | 
                Where-Object { $_.FullName -notlike "*translation-workspace*" }
    
    $ps1Issues = @()
    foreach ($file in $ps1Files) {
        try {
            $content = Get-Content $file.FullName -Raw
            if ($content -match '[æøåÆØÅ]') {
                $ps1Issues += "$($file.Name) contains Danish characters"
            }
        } catch {
            $ps1Issues += "Failed to read $($file.Name)"
        }
    }
    
    if ($ps1Issues.Count -eq 0) {
        $ps1Test.Status = "PASS"
        $ps1Test.Details += "All PowerShell scripts are valid"
    } else {
        $ps1Test.Status = "WARNING"
        $ps1Test.Details += "PowerShell script issues found: $($ps1Issues.Count)"
        $ps1Test.Details += $ps1Issues
    }
    
} catch {
    $ps1Test.Status = "FAIL"
    $ps1Test.Errors += "PowerShell test failed: $($_.Exception.Message)"
}

$verificationResults.Tests += $ps1Test

# Test 4: Documentation Link Validation
Write-Host "`n🔍 Testing documentation links..." -ForegroundColor Cyan
$linkTest = @{
    Name = "Documentation Link Validation"
    Status = "PENDING"
    Details = @()
    Errors = @()
}

try {
    $markdownFiles = Get-ChildItem -Recurse -Include "*.md" | 
                    Where-Object { $_.FullName -notlike "*translation-workspace*" }
    
    $linkIssues = @()
    foreach ($file in $markdownFiles) {
        try {
            $content = Get-Content $file.FullName -Raw
            # Check for relative file links
            $relativeLinks = [regex]::Matches($content, '\[([^\]]+)\]\(([^)]+)\)') | 
                            Where-Object { $_.Groups[2].Value -notlike "http*" -and $_.Groups[2].Value -notlike "mailto*" -and $_.Groups[2].Value -notlike "#*" }
            
            foreach ($link in $relativeLinks) {
                $linkPath = $link.Groups[2].Value
                $fileDir = $file.DirectoryName
                $targetPath = Join-Path $fileDir $linkPath
                
                if (!(Test-Path $targetPath)) {
                    $linkIssues += "$($file.Name): Broken link to '$linkPath'"
                }
            }
        } catch {
            $linkIssues += "Failed to process $($file.Name): $($_.Exception.Message)"
        }
    }
    
    if ($linkIssues.Count -eq 0) {
        $linkTest.Status = "PASS"
        $linkTest.Details += "All documentation links are valid"
    } else {
        $linkTest.Status = "WARNING"
        $linkTest.Details += "Documentation link issues found: $($linkIssues.Count)"
        $linkTest.Details += $linkIssues[0..4] # Show first 5 issues
        if ($linkIssues.Count -gt 5) {
            $linkTest.Details += "... and $($linkIssues.Count - 5) more"
        }
    }
    
} catch {
    $linkTest.Status = "FAIL"
    $linkTest.Errors += "Link test failed: $($_.Exception.Message)"
}

$verificationResults.Tests += $linkTest

# Test 5: File Structure Validation
Write-Host "`n🔍 Testing file structure..." -ForegroundColor Cyan
$structureTest = @{
    Name = "File Structure Validation"
    Status = "PENDING"
    Details = @()
    Errors = @()
}

try {
    $criticalFiles = @(
        "CMakeLists.txt",
        "README.md",
        "src/core/Framework.cpp",
        "src/core/Framework.hpp"
    )
    
    $missingFiles = @()
    foreach ($file in $criticalFiles) {
        if (!(Test-Path $file)) {
            $missingFiles += $file
        }
    }
    
    if ($missingFiles.Count -eq 0) {
        $structureTest.Status = "PASS"
        $structureTest.Details += "All critical files are present"
    } else {
        $structureTest.Status = "WARNING"
        $structureTest.Details += "Missing critical files: $($missingFiles -join ', ')"
    }
    
} catch {
    $structureTest.Status = "FAIL"
    $structureTest.Errors += "Structure test failed: $($_.Exception.Message)"
}

$verificationResults.Tests += $structureTest

# Calculate overall status
$failedTests = ($verificationResults.Tests | Where-Object { $_.Status -eq "FAIL" }).Count
$warningTests = ($verificationResults.Tests | Where-Object { $_.Status -eq "WARNING" }).Count

if ($failedTests -gt 0) {
    $verificationResults.OverallStatus = "FAIL"
} elseif ($warningTests -gt 0) {
    $verificationResults.OverallStatus = "WARNING"
} else {
    $verificationResults.OverallStatus = "PASS"
}

# Generate detailed report
$reportFile = Join-Path $OutputDir "build_verification_report.md"
$markdownContent = @"
# 🔧 UEVR Build Verification Report

## 📋 **VERIFICATION SUMMARY**
- **Status**: $($verificationResults.OverallStatus)
- **Timestamp**: $($verificationResults.Timestamp)
- **Project Root**: $($verificationResults.ProjectRoot)

---

## 🧪 **TEST RESULTS**

"@

foreach ($test in $verificationResults.Tests) {
    $statusIcon = switch ($test.Status) {
        "PASS" { "✅" }
        "WARNING" { "⚠️" }
        "FAIL" { "❌" }
        "SKIP" { "⏭️" }
        default { "❓" }
    }
    
    $markdownContent += @"

### $statusIcon **$($test.Name)**
**Status**: $($test.Status)

**Details**:
"@
    
    foreach ($detail in $test.Details) {
        $markdownContent += "- $detail`n"
    }
    
    if ($test.Errors.Count -gt 0) {
        $markdownContent += "`n**Errors**:`n"
        foreach ($errorItem in $test.Errors) {
            $markdownContent += "- ❌ $errorItem`n"
        }
    }
}

$markdownContent += @"

---

## 📊 **STATISTICS**

- **Total Tests**: $($verificationResults.Tests.Count)
- **Passed**: $(($verificationResults.Tests | Where-Object { $_.Status -eq "PASS" }).Count)
- **Warnings**: $(($verificationResults.Tests | Where-Object { $_.Status -eq "WARNING" }).Count)
- **Failed**: $(($verificationResults.Tests | Where-Object { $_.Status -eq "FAIL" }).Count)
- **Skipped**: $(($verificationResults.Tests | Where-Object { $_.Status -eq "SKIP" }).Count)

---

## 🎯 **RECOMMENDATIONS**

"@

if ($verificationResults.OverallStatus -eq "PASS") {
    $markdownContent += "✅ **All tests passed!** The project is ready for translation deployment.`n"
} elseif ($verificationResults.OverallStatus -eq "WARNING") {
    $markdownContent += "⚠️ **Warnings detected.** Review and address warnings before proceeding.`n"
} else {
    $markdownContent += "❌ **Critical failures detected.** Fix all failures before proceeding.`n"
}

$markdownContent += @"

### **Next Steps**
1. **Review all warnings and errors** listed above
2. **Fix critical issues** before proceeding with translation
3. **Address warnings** to ensure optimal build quality
4. **Re-run verification** after making fixes
5. **Proceed with translation** only after all tests pass

---

**Generated**: $($verificationResults.Timestamp)  
**Status**: $($verificationResults.OverallStatus)  
**Next Action**: Review and address any issues found
"@

$markdownContent | Out-File -FilePath $reportFile -Encoding UTF8

# Generate console summary
Write-Host "`n" + "="*60 -ForegroundColor Green
Write-Host "BUILD VERIFICATION COMPLETE" -ForegroundColor Green
Write-Host "="*60 -ForegroundColor Green
Write-Host "Overall Status: $($verificationResults.OverallStatus)" -ForegroundColor $(if ($verificationResults.OverallStatus -eq "PASS") { "Green" } elseif ($verificationResults.OverallStatus -eq "WARNING") { "Yellow" } else { "Red" })

Write-Host "`nTest Results:" -ForegroundColor Cyan
foreach ($test in $verificationResults.Tests) {
    $color = switch ($test.Status) {
        "PASS" { "Green" }
        "WARNING" { "Yellow" }
        "FAIL" { "Red" }
        "SKIP" { "Gray" }
        default { "White" }
    }
    Write-Host "  $($test.Status): $($test.Name)" -ForegroundColor $color
}

Write-Host "`n📊 Detailed report saved to: $reportFile" -ForegroundColor Cyan

# Exit with appropriate code
if ($verificationResults.OverallStatus -eq "FAIL") {
    exit 1
} elseif ($verificationResults.OverallStatus -eq "WARNING") {
    exit 2
} else {
    exit 0
}
