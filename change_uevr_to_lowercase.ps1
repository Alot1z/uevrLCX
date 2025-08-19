# PowerShell Script: Change UEVR to uevr throughout the entire codebase
# This script performs a comprehensive search and replace operation

Write-Host "Starting UEVR to uevr conversion script..." -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Green

# Get the workspace root (go up from tools directory to the main UEVR directory)
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$workspaceRoot = Split-Path -Parent $scriptPath
Write-Host "Script location: $scriptPath" -ForegroundColor Yellow
Write-Host "Workspace root: $workspaceRoot" -ForegroundColor Yellow

# Verify we're in the right place
if (-not (Test-Path "$workspaceRoot\src") -and -not (Test-Path "$workspaceRoot\include")) {
    Write-Host "ERROR: Could not find src or include directories. Make sure this script is run from the UEVR workspace root." -ForegroundColor Red
    exit 1
}

# Define file extensions to process (exclude binary files and build artifacts)
$fileExtensions = @(
    "*.md", "*.txt", "*.hpp", "*.h", "*.cpp", "*.c", "*.cmake", 
    "*.bat", "*.ps1", "*.json", "*.toml", "*.ini", "*.cfg"
)

# Define directories to exclude
$excludeDirs = @(
    "build", "builds", "build_*", "bin", "obj", "Debug", "Release",
    "node_modules", ".git", ".vs", "*.user", "*.suo", "*.cache"
)

# Counter for processed files
$processedFiles = 0
$modifiedFiles = 0

Write-Host "Searching for files to process..." -ForegroundColor Cyan

# Get all files recursively, excluding build directories
$allFiles = Get-ChildItem -Path $workspaceRoot -Recurse -File | Where-Object {
    $file = $_
    $shouldInclude = $true
    
    # Check if file extension matches our target types
    $extensionMatch = $false
    foreach ($ext in $fileExtensions) {
        if ($file.Name -like $ext) {
            $extensionMatch = $true
            break
        }
    }
    
    if (-not $extensionMatch) {
        $shouldInclude = $false
    }
    
    # Check if file is in excluded directory
    foreach ($excludeDir in $excludeDirs) {
        if ($file.FullName -like "*\$excludeDir\*" -or $file.FullName -like "*\$excludeDir") {
            $shouldInclude = $false
            break
        }
    }
    
    return $shouldInclude
}

Write-Host "Found $($allFiles.Count) files to process" -ForegroundColor Cyan

# Process each file
foreach ($file in $allFiles) {
    try {
        $content = Get-Content -Path $file.FullName -Raw -Encoding UTF8
        $originalContent = $content
        
        # Check if file contains "UEVR" (case-sensitive)
        if ($content -match "UEVR") {
            Write-Host "Processing: $($file.Name)" -ForegroundColor Yellow
            
            # Replace all instances of "UEVR" with "uevr"
            $content = $content -replace "UEVR", "uevr"
            
            # Check if content actually changed
            if ($content -ne $originalContent) {
                # Write the modified content back to the file
                Set-Content -Path $file.FullName -Value $content -Encoding UTF8 -NoNewline
                Write-Host "  Modified: $($file.Name)" -ForegroundColor Green
                $modifiedFiles++
            }
        }
        
        $processedFiles++
        
        # Progress indicator
        if ($processedFiles % 100 -eq 0) {
            Write-Host "  Processed $processedFiles files..." -ForegroundColor Gray
        }
        
    } catch {
        Write-Host "  Error processing $($file.Name): $($_.Exception.Message)" -ForegroundColor Red
    }
}

Write-Host "================================================" -ForegroundColor Green
Write-Host "Conversion completed!" -ForegroundColor Green
Write-Host "Summary:" -ForegroundColor Cyan
Write-Host "  - Total files processed: $processedFiles" -ForegroundColor White
Write-Host "  - Files modified: $modifiedFiles" -ForegroundColor White
Write-Host "  - Files unchanged: $($processedFiles - $modifiedFiles)" -ForegroundColor White

# Show some examples of what was changed
Write-Host "`nChecking for remaining UEVR references..." -ForegroundColor Cyan
$remainingUEVR = Get-ChildItem -Path $workspaceRoot -Recurse -File -Include $fileExtensions | 
    Where-Object { (Get-Content $_.FullName -Raw) -match "UEVR" } |
    Select-Object -First 5

if ($remainingUEVR.Count -gt 0) {
    Write-Host "Found $($remainingUEVR.Count) files that still contain 'UEVR':" -ForegroundColor Yellow
    foreach ($file in $remainingUEVR) {
        Write-Host "  - $($file.FullName)" -ForegroundColor Gray
    }
} else {
    Write-Host "No remaining 'UEVR' references found!" -ForegroundColor Green
}

Write-Host "`nScript execution completed!" -ForegroundColor Green
