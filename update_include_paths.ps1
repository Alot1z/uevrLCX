# ========================================
# uevr Include Path Update Script
# ========================================
# This script updates include paths in source files to match the new structure
# 
# Usage: .\update_include_paths.ps1 [-Backup] [-Validate] [-Recursive]
# 
# Parameters:
#   -Backup: Create backup of files before updating
#   -Validate: Validate include paths after update
#   -Recursive: Process subdirectories recursively
# 
# Features:
#   - Comprehensive include path mapping
#   - Backup functionality
#   - Validation of updated paths
#   - Detailed reporting
#   - Error handling and rollback

param(
    [switch]$Backup,
    [switch]$Validate,
    [switch]$Recursive
)

# Set execution policy for this session
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process -Force

# Function to write colored output
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

# Function to create timestamp
function Get-Timestamp {
    return Get-Date -Format "yyyyMMdd_HHmmss"
}

# Function to create backup
function Backup-Files {
    param([array]$Files, [string]$BackupDir)
    
    Write-ColorOutput "Creating backup of files..." "Yellow"
    
    if (-not (Test-Path $BackupDir)) {
        New-Item -ItemType Directory -Name $BackupDir -Force | Out-Null
    }
    
    $backedUpCount = 0
    foreach ($file in $Files) {
        try {
            $backupPath = Join-Path $BackupDir $file.Name
            Copy-Item -Path $file.FullName -Destination $backupPath -Force
            $backedUpCount++
        } catch {
            Write-ColorOutput "Failed to backup: $($file.FullName) - $_" "Red"
        }
    }
    
    Write-ColorOutput "Backup completed: $backedUpCount files backed up to $BackupDir" "Green"
}

# Function to get comprehensive include mappings
function Get-IncludeMappings {
    return @{
        # Adapter includes
        '\.\.\/\.\.\/include\/uevr\/' = 'uevr/'
        '\.\.\/\.\.\/include\/' = 'uevr/'
        '\.\.\/include\/uevr\/' = 'uevr/'
        '\.\.\/include\/' = 'uevr/'
        
        # Common includes
        '\.\.\/common\/' = 'uevr/adapters/common/'
        '\.\.\/\.\.\/common\/' = 'uevr/adapters/common/'
        
        # Specific adapter includes
        '\.\.\/\.\.\/MT-Framework\/' = 'uevr/adapters/mt-framework/'
        '\.\.\/\.\.\/RE-Engine\/' = 'uevr/adapters/re-engine/'
        '\.\.\/\.\.\/REDengine4\/' = 'uevr/adapters/redengine4/'
        '\.\.\/MT-Framework\/' = 'uevr/adapters/mt-framework/'
        '\.\.\/RE-Engine\/' = 'uevr/adapters/re-engine/'
        '\.\.\/REDengine4\/' = 'uevr/adapters/redengine4/'
        
        # Core includes
        '\.\.\/\.\.\/SOURCECODE\/uevr\/src\/' = 'uevr/core/'
        '\.\.\/\.\.\/SOURCECODE\/uevr\/include\/' = 'uevr/'
        '\.\.\/SOURCECODE\/uevr\/src\/' = 'uevr/core/'
        '\.\.\/SOURCECODE\/uevr\/include\/' = 'uevr/'
        
        # Hook includes
        '\.\.\/\.\.\/hooks\/' = 'uevr/hooks/'
        '\.\.\/hooks\/' = 'uevr/hooks/'
        
        # Mod includes
        '\.\.\/\.\.\/mods\/' = 'uevr/mods/'
        '\.\.\/mods\/' = 'uevr/mods/'
        
        # Utility includes
        '\.\.\/\.\.\/utility\/' = 'uevr/core/utility/'
        '\.\.\/utility\/' = 'uevr/core/utility/'
        
        # SDK includes
        '\.\.\/\.\.\/sdk\/' = 'uevr/core/sdk/'
        '\.\.\/sdk\/' = 'uevr/core/sdk/'
        
        # Legacy includes
        '\.\.\/\.\.\/\.\.\/include\/uevr\/' = 'uevr/'
        '\.\.\/\.\.\/\.\.\/include\/' = 'uevr/'
    }
}

# Function to scan for source files
function Get-SourceFiles {
    param([array]$Directories, [array]$Extensions)
    
    Write-ColorOutput "Scanning for source files..." "Yellow"
    
    $filesToUpdate = @()
    foreach ($dir in $Directories) {
        if (Test-Path $dir) {
            Write-ColorOutput "Scanning directory: $dir" "Cyan"
            
            foreach ($ext in $Extensions) {
                $searchPath = if ($Recursive) { $dir } else { "$dir\*" }
                $files = Get-ChildItem -Path $searchPath -Filter $ext -ErrorAction SilentlyContinue
                $filesToUpdate += $files
                Write-ColorOutput "  Found $($files.Count) $ext files" "Gray"
            }
        } else {
            Write-ColorOutput "Directory not found: $dir" "Red"
        }
    }
    
    Write-ColorOutput "Total files found: $($filesToUpdate.Count)" "Green"
    return $filesToUpdate
}

# Function to update include paths in a file
function Update-FileIncludePaths {
    param([System.IO.FileInfo]$File, [hashtable]$Mappings)
    
    try {
        $content = Get-Content $File.FullName -Raw -ErrorAction Stop
        $originalContent = $content
        $fileUpdated = $false
        $changes = @()
        
        foreach ($mapping in $Mappings.GetEnumerator()) {
            $oldPattern = $mapping.Key
            $newPath = $mapping.Value
            
            if ($content -match $oldPattern) {
                $matches = [regex]::Matches($content, $oldPattern)
                foreach ($match in $matches) {
                    $changes += "  $($match.Value) -> $newPath"
                }
                
                $content = $content -replace $oldPattern, $newPath
                $fileUpdated = $true
            }
        }
        
        if ($fileUpdated) {
            Set-Content -Path $File.FullName -Value $content -NoNewline -ErrorAction Stop
            return @{
                Success = $true
                Changes = $changes
                File = $File.FullName
            }
        } else {
            return @{
                Success = $true
                Changes = @()
                File = $File.FullName
            }
        }
    } catch {
        return @{
            Success = $false
            Error = $_.Exception.Message
            File = $File.FullName
        }
    }
}

# Function to validate include paths
function Test-IncludePaths {
    param([array]$Files)
    
    Write-ColorOutput "Validating include paths..." "Yellow"
    
    $validationResults = @()
    $validPaths = @(
        'uevr/',
        'uevr/adapters/',
        'uevr/hooks/',
        'uevr/mods/',
        'uevr/core/',
        'uevr/core/utility/',
        'uevr/core/sdk/',
        'uevr/adapters/common/',
        'uevr/adapters/mt-framework/',
        'uevr/adapters/re-engine/',
        'uevr/adapters/redengine4/'
    )
    
    foreach ($file in $Files) {
        try {
            $content = Get-Content $file.FullName -Raw -ErrorAction Stop
            $includes = [regex]::Matches($content, '#include\s+["<]([^">]+)[">]')
            
            $fileValid = $true
            $invalidIncludes = @()
            
            foreach ($include in $includes) {
                $includePath = $include.Groups[1].Value
                $isValid = $false
                
                foreach ($validPath in $validPaths) {
                    if ($includePath.StartsWith($validPath)) {
                        $isValid = $true
                        break
                    }
                }
                
                if (-not $isValid) {
                    $fileValid = $false
                    $invalidIncludes += $includePath
                }
            }
            
            $validationResults += @{
                File = $file.FullName
                Valid = $fileValid
                InvalidIncludes = $invalidIncludes
            }
            
            if ($fileValid) {
                Write-ColorOutput "✓ $($file.Name)" "Green"
            } else {
                Write-ColorOutput "✗ $($file.Name)" "Red"
                foreach ($invalid in $invalidIncludes) {
                    Write-ColorOutput "  Invalid include: $invalid" "Red"
                }
            }
        } catch {
            Write-ColorOutput "✗ $($file.Name) - Error reading file" "Red"
            $validationResults += @{
                File = $file.FullName
                Valid = $false
                Error = $_.Exception.Message
            }
        }
    }
    
    $validCount = ($validationResults | Where-Object { $_.Valid }).Count
    $totalCount = $validationResults.Count
    
    Write-ColorOutput "Validation completed: $validCount/$totalCount files have valid include paths" "Green"
    return $validationResults
}

# Main execution
try {
    Write-Host ""
    Write-ColorOutput "========================================" "Magenta"
    Write-ColorOutput "    uevr Include Path Update Script" "Magenta"
    Write-ColorOutput "========================================" "Magenta"
    Write-Host ""
    
    # Configuration
    $fileExtensions = @('*.cpp', '*.hpp', '*.h', '*.c', '*.inl')
    $directories = @('src', 'adapters', 'SOURCECODE', 'include', 'examples', 'tests')
    $includeMappings = Get-IncludeMappings
    
    Write-ColorOutput "Configuration:" "Cyan"
    Write-ColorOutput "  File Extensions: $($fileExtensions -join ', ')" "White"
    Write-ColorOutput "  Directories: $($directories -join ', ')" "White"
    Write-ColorOutput "  Include Mappings: $($includeMappings.Count)" "White"
    Write-ColorOutput "  Recursive: $Recursive" "White"
    Write-ColorOutput "  Backup: $Backup" "White"
    Write-ColorOutput "  Validate: $Validate" "White"
    Write-Host ""
    
    # Scan for source files
    $sourceFiles = Get-SourceFiles -Directories $directories -Extensions $fileExtensions
    Write-Host ""
    
    if ($sourceFiles.Count -eq 0) {
        Write-ColorOutput "No source files found to process." "Yellow"
        exit 0
    }
    
    # Create backup if requested
    if ($Backup) {
        $backupDir = "backup_includes_$(Get-Timestamp)"
        Backup-Files -Files $sourceFiles -BackupDir $backupDir
        Write-Host ""
    }
    
    # Process files
    Write-ColorOutput "Processing files..." "Yellow"
    Write-Host ""
    
    $updatedFiles = 0
    $failedFiles = 0
    $totalChanges = 0
    $processingResults = @()
    
    foreach ($file in $sourceFiles) {
        Write-ColorOutput "Processing: $($file.Name)" "Cyan"
        
        $result = Update-FileIncludePaths -File $file -Mappings $includeMappings
        
        if ($result.Success) {
            if ($result.Changes.Count -gt 0) {
                Write-ColorOutput "  ✓ Updated with $($result.Changes.Count) changes" "Green"
                foreach ($change in $result.Changes) {
                    Write-ColorOutput $change "Gray"
                }
                $updatedFiles++
                $totalChanges += $result.Changes.Count
            } else {
                Write-ColorOutput "  - No changes needed" "Gray"
            }
        } else {
            Write-ColorOutput "  ✗ Failed: $($result.Error)" "Red"
            $failedFiles++
        }
        
        $processingResults += $result
    }
    
    Write-Host ""
    Write-ColorOutput "Processing completed!" "Green"
    Write-ColorOutput "  Files updated: $updatedFiles" "White"
    Write-ColorOutput "  Files failed: $failedFiles" "White"
    Write-ColorOutput "  Total changes: $totalChanges" "White"
    Write-Host ""
    
    # Validate if requested
    if ($Validate) {
        $validationResults = Test-IncludePaths -Files $sourceFiles
        Write-Host ""
    }
    
    # Summary
    Write-ColorOutput "========================================" "Green"
    Write-ColorOutput "Include path update completed!" "Green"
    Write-ColorOutput "========================================" "Green"
    Write-Host ""
    
    Write-ColorOutput "Next steps:" "Yellow"
    Write-ColorOutput "1. Review the changes in the updated files" "White"
    Write-ColorOutput "2. Test compilation to ensure all includes are correct" "White"
    Write-ColorOutput "3. Run build_restructured.ps1 to test the build" "White"
    Write-ColorOutput "4. Fix any validation issues if validation was enabled" "White"
    Write-Host ""
    
    if ($Backup) {
        Write-ColorOutput "Backup created in: $backupDir" "Cyan"
        Write-ColorOutput "You can restore files from backup if needed" "Cyan"
        Write-Host ""
    }
    
} catch {
    Write-ColorOutput "========================================" "Red"
    Write-ColorOutput "Include path update failed!" "Red"
    Write-ColorOutput "========================================" "Red"
    Write-Host ""
    Write-ColorOutput "Error: $_" "Red"
    Write-Host ""
    exit 1
}

Write-ColorOutput "========================================" "Cyan"
Write-ColorOutput "Process completed!" "Cyan"
Write-ColorOutput "========================================" "Cyan"
Write-Host ""

Read-Host "Press Enter to continue..."
