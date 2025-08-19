# PowerShell Script: Change UEVR to uevr throughout the entire codebase
# This script performs a comprehensive search and replace operation
# Integrated with "Vibe and Replace" functionality

Write-Host "Starting UEVR to uevr conversion script..." -ForegroundColor Green
Write-Host "=================================================" -ForegroundColor Green

# Get the workspace root (find the UEVR directory)
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$currentPath = $scriptPath

# Look for the UEVR directory by going up the directory tree
while ($currentPath -ne (Split-Path -Parent $currentPath)) {
    if ((Split-Path -Leaf $currentPath) -eq "UEVR") {
        $workspaceRoot = $currentPath
        break
    }
    $currentPath = Split-Path -Parent $currentPath
}

# If we didn't find it, use the script path and go up one level
if (-not $workspaceRoot) {
    $workspaceRoot = Split-Path -Parent $scriptPath
}

Write-Host "Script location: $scriptPath" -ForegroundColor Yellow
Write-Host "Workspace root: $workspaceRoot" -ForegroundColor Yellow

# Verify we're in the right place
if (-not (Test-Path "$workspaceRoot\src") -and -not (Test-Path "$workspaceRoot\include")) {
    Write-Host "ERROR: Could not find src or include directories. Make sure this script is run from the UEVR workspace root." -ForegroundColor Red
    exit 1
}

# Change to workspace root
Set-Location $workspaceRoot
Write-Host "Changed to workspace root: $(Get-Location)" -ForegroundColor Green

# Define file extensions to process (exclude binary files and build artifacts)
$fileExtensions = @(
    "*.md", "*.txt", "*.hpp", "*.h", "*.cpp", "*.c", "*.cmake", 
    "*.bat", "*.ps1", "*.json", "*.toml", "*.ini", "*.cfg"
)

# Define directories to exclude
$excludeDirs = @(
    "build", "builds", "build_artifacts", "build_reports", "build_system_results",
    "node_modules", ".git", "bin", "obj", "Debug", "Release", "x64", "x86"
)

Write-Host "Searching for files to process..." -ForegroundColor Yellow

# Get all files recursively, excluding build directories
$allFiles = @()
foreach ($ext in $fileExtensions) {
    $files = Get-ChildItem -Path $workspaceRoot -Recurse -Include $ext -ErrorAction SilentlyContinue | 
             Where-Object { 
                 $dir = Split-Path $_.DirectoryName -Leaf
                 $excludeDirs -notcontains $dir -and 
                 $_.DirectoryName -notmatch "\\build\\" -and
                 $_.DirectoryName -notmatch "\\builds\\" -and
                 $_.DirectoryName -notmatch "\\build_artifacts\\" -and
                 $_.DirectoryName -notmatch "\\build_reports\\" -and
                 $_.DirectoryName -notmatch "\\build_system_results\\"
             }
    $allFiles += $files
}

Write-Host "Found $($allFiles.Count) files to process" -ForegroundColor Green

# Initialize counters
$processedFiles = 0
$modifiedFiles = 0
$totalReplacements = 0

Write-Host "=================================================" -ForegroundColor Green

# Process each file
foreach ($file in $allFiles) {
    $processedFiles++
    
    # Show progress
    if ($processedFiles % 100 -eq 0) {
        Write-Host "Processed $processedFiles of $($allFiles.Count) files..." -ForegroundColor Yellow
    }
    
    Write-Host "Processing: $($file.Name)" -ForegroundColor Cyan
    
    try {
        # Try to read the file content
        $content = $null
        $encoding = $null
        
        # Try different encoding methods
        try {
            $content = Get-Content $file.FullName -Raw -Encoding UTF8 -ErrorAction Stop
            $encoding = "UTF8"
        } catch {
            try {
                $content = Get-Content $file.FullName -Raw -Encoding Default -ErrorAction Stop
                $encoding = "Default"
            } catch {
                try {
                    $content = Get-Content $file.FullName -Raw -ErrorAction Stop
                    $encoding = "NoEncoding"
                } catch {
                    Write-Host "  ERROR: Could not read file: $($file.FullName)" -ForegroundColor Red
                    continue
                }
            }
        }
        
        if (-not $content) {
            Write-Host "  WARNING: File is empty or could not be read: $($file.FullName)" -ForegroundColor Yellow
            continue
        }
        
        # Count UEVR references before replacement
        $uevrCountBefore = ([regex]::Matches($content, "UEVR", [System.Text.RegularExpressions.RegexOptions]::IgnoreCase)).Count
        
        if ($uevrCountBefore -eq 0) {
            Write-Host "  Found 0 UEVR references" -ForegroundColor Gray
            Write-Host "  No changes needed: $($file.Name)" -ForegroundColor Gray
            continue
        }
        
        Write-Host "  Found $uevrCountBefore UEVR references" -ForegroundColor Yellow
        
        # Try multiple replacement methods
        $newContent = $null
        $replacementMethod = ""
        
        # Method 1: PowerShell -replace operator
        try {
            $testContent = $content -replace "UEVR", "uevr"
            if ($testContent -ne $content) {
                $newContent = $testContent
                $replacementMethod = "PowerShell -replace"
            }
        } catch {
            Write-Host "  PowerShell -replace failed: $($_.Exception.Message)" -ForegroundColor Red
        }
        
        # Method 2: .NET Regex.Replace
        if (-not $newContent) {
            try {
                $testContent = [regex]::Replace($content, "UEVR", "uevr", [System.Text.RegularExpressions.RegexOptions]::IgnoreCase)
                if ($testContent -ne $content) {
                    $newContent = $testContent
                    $replacementMethod = ".NET Regex.Replace"
                }
            } catch {
                Write-Host "  .NET Regex.Replace failed: $($_.Exception.Message)" -ForegroundColor Red
            }
        }
        
        # Method 3: String.Replace
        if (-not $newContent) {
            try {
                $testContent = $content.Replace("UEVR", "uevr")
                if ($testContent -ne $content) {
                    $newContent = $testContent
                    $replacementMethod = "String.Replace"
                }
            } catch {
                Write-Host "  String.Replace failed: $($_.Exception.Message)" -ForegroundColor Red
            }
        }
        
        # Method 4: Manual character-by-character replacement
        if (-not $newContent) {
            try {
                $testContent = $content
                $testContent = $testContent.Replace("UEVR", "uevr")
                $testContent = $testContent.Replace("Uevr", "uevr")
                $testContent = $testContent.Replace("uevr", "uevr")
                if ($testContent -ne $content) {
                    $newContent = $testContent
                    $replacementMethod = "Manual character replacement"
                }
            } catch {
                Write-Host "  Manual replacement failed: $($_.Exception.Message)" -ForegroundColor Red
            }
        }
        
        if (-not $newContent) {
            Write-Host "  ERROR: All replacement methods failed!" -ForegroundColor Red
            Write-Host "  Original content sample: $($content.Substring(0, [Math]::Min(100, $content.Length)))" -ForegroundColor Red
            continue
        }
        
        Write-Host "  Replacement successful using: $replacementMethod" -ForegroundColor Green
        
        # Count UEVR references after replacement
        $uevrCountAfter = ([regex]::Matches($newContent, "UEVR", [System.Text.RegularExpressions.RegexOptions]::IgnoreCase)).Count
        
        if ($uevrCountAfter -eq 0) {
            # All UEVR references were replaced
            $replacementsMade = $uevrCountBefore - $uevrCountAfter
            
            # Try to write the file using multiple methods
            $writeSuccess = $false
            
            # Method 1: Set-Content with UTF8 encoding and Force
            try {
                # First, try to remove read-only attribute if it exists
                if ((Get-ItemProperty $file.FullName).IsReadOnly) {
                    Set-ItemProperty $file.FullName -Name IsReadOnly -Value $false
                    Write-Host "  Removed read-only attribute" -ForegroundColor Yellow
                }
                
                Set-Content -Path $file.FullName -Value $newContent -Encoding UTF8 -Force -ErrorAction Stop
                $writeSuccess = $true
                Write-Host "  SUCCESS: File written using Set-Content UTF8" -ForegroundColor Green
            } catch {
                Write-Host "  Method 1 failed: $($_.Exception.Message)" -ForegroundColor Red
            }
            
            # Method 2: Set-Content with Default encoding and Force
            if (-not $writeSuccess) {
                try {
                    Set-Content -Path $file.FullName -Value $newContent -Encoding Default -Force -ErrorAction Stop
                    $writeSuccess = $true
                    Write-Host "  SUCCESS: File written using Set-Content Default" -ForegroundColor Green
                } catch {
                    Write-Host "  Method 2 failed: $($_.Exception.Message)" -ForegroundColor Red
                }
            }
            
            # Method 3: Out-File with UTF8 encoding and Force
            if (-not $writeSuccess) {
                try {
                    $newContent | Out-File -FilePath $file.FullName -Encoding UTF8 -Force -ErrorAction Stop
                    $writeSuccess = $true
                    Write-Host "  SUCCESS: File written using Out-File UTF8" -ForegroundColor Green
                } catch {
                    Write-Host "  Method 3 failed: $($_.Exception.Message)" -ForegroundColor Red
                }
            }
            
            # Method 4: Try using .NET StreamWriter directly
            if (-not $writeSuccess) {
                try {
                    $stream = [System.IO.File]::Open($file.FullName, [System.IO.FileMode]::Truncate, [System.IO.FileAccess]::Write, [System.IO.FileShare]::None)
                    $writer = [System.IO.StreamWriter]::new($stream, [System.Text.UTF8Encoding]::new($false))
                    $writer.Write($newContent)
                    $writer.Close()
                    $stream.Close()
                    $writeSuccess = $true
                    Write-Host "  SUCCESS: File written using .NET StreamWriter" -ForegroundColor Green
                } catch {
                    Write-Host "  Method 4 failed: $($_.Exception.Message)" -ForegroundColor Red
                }
            }
            
            # Method 5: Try using .NET File.WriteAllText
            if (-not $writeSuccess) {
                try {
                    [System.IO.File]::WriteAllText($file.FullName, $newContent, [System.Text.UTF8Encoding]::new($false))
                    $writeSuccess = $true
                    Write-Host "  SUCCESS: File written using .NET File.WriteAllText" -ForegroundColor Green
                } catch {
                    Write-Host "  Method 5 failed: $($_.Exception.Message)" -ForegroundColor Red
                }
            }
            
            if ($writeSuccess) {
                $modifiedFiles++
                $totalReplacements += $replacementsMade
                Write-Host "  MODIFIED: $($file.Name) - $replacementsMade replacements made" -ForegroundColor Green
            } else {
                Write-Host "  ERROR: Could not write file: $($file.FullName)" -ForegroundColor Red
                Write-Host "  All write methods failed. File may be locked or have permission issues." -ForegroundColor Red
            }
        } else {
            Write-Host "  WARNING: Some UEVR references remain ($uevrCountAfter left)" -ForegroundColor Yellow
            Write-Host "  This suggests the replacement didn't work as expected" -ForegroundColor Yellow
            Write-Host "  Original UEVR count: $uevrCountBefore, After replacement: $uevrCountAfter" -ForegroundColor Yellow
        }
        
    } catch {
        Write-Host "  ERROR processing file: $($_.Exception.Message)" -ForegroundColor Red
    }
}

Write-Host "=================================================" -ForegroundColor Green
Write-Host "Conversion completed!" -ForegroundColor Green
Write-Host "Summary:" -ForegroundColor Green
Write-Host "  - Total files processed: $processedFiles" -ForegroundColor White
Write-Host "  - Files modified: $modifiedFiles" -ForegroundColor White
Write-Host "  - Total replacements made: $totalReplacements" -ForegroundColor White

# Final verification - check for remaining UEVR references
Write-Host "`nChecking for remaining UEVR references..." -ForegroundColor Yellow
$remainingFiles = @()

foreach ($file in $allFiles) {
    try {
        $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
        if ($content -and $content -match "UEVR") {
            $uevrCount = ([regex]::Matches($content, "UEVR", [System.Text.RegularExpressions.RegexOptions]::IgnoreCase)).Count
            $remainingFiles += @{
                File = $file.FullName
                Count = $uevrCount
            }
        }
    } catch {
        # Skip files that can't be read
    }
}

if ($remainingFiles.Count -gt 0) {
    Write-Host "Found $($remainingFiles.Count) files that still contain 'UEVR':" -ForegroundColor Red
    foreach ($fileInfo in $remainingFiles) {
        Write-Host "  - $($fileInfo.File) ($($fileInfo.Count) references)" -ForegroundColor Red
    }
    
    Write-Host "`nTroubleshooting tips:" -ForegroundColor Yellow
    Write-Host "1. Make sure no files are open in other applications" -ForegroundColor Yellow
    Write-Host "2. Try running the script as Administrator" -ForegroundColor Yellow
    Write-Host "3. Check if files have read-only attributes" -ForegroundColor Yellow
    Write-Host "4. Some files may be locked by the system" -ForegroundColor Yellow
} else {
    Write-Host "All UEVR references have been successfully converted to uevr!" -ForegroundColor Green
}

Write-Host "`nScript execution completed!" -ForegroundColor Green
