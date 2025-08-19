# ========================================
# uevr Project Structure Migration Script
# ========================================
# This script migrates files from the old structure to the new restructured layout
# 
# Usage: .\migrate_to_new_structure.ps1 [-Backup] [-Force]
# 
# Parameters:
#   -Backup: Create backup of original files before migration
#   -Force: Overwrite existing files without confirmation
# 
# Features:
#   - Creates new directory structure
#   - Migrates source files automatically
#   - Updates include paths
#   - Creates backup of original files
#   - Validates migration results

param(
    [switch]$Backup,
    [switch]$Force
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
function Backup-OriginalFiles {
    param([string]$BackupDir)
    
    Write-ColorOutput "Creating backup of original files..." "Yellow"
    
    $backupSources = @(
        "SOURCECODE",
        "adapters",
        "include",
        "src"
    )
    
    foreach ($source in $backupSources) {
        if (Test-Path $source) {
            $backupPath = Join-Path $BackupDir $source
            Write-ColorOutput "Backing up: $source -> $backupPath" "Cyan"
            
            try {
                Copy-Item -Path $source -Destination $backupPath -Recurse -Force
                Write-ColorOutput "  ✓ Backup completed" "Green"
            } catch {
                Write-ColorOutput "  ✗ Backup failed: $_" "Red"
            }
        }
    }
    
    Write-ColorOutput "Backup completed: $BackupDir" "Green"
}

# Function to create directory structure
function New-DirectoryStructure {
    Write-ColorOutput "Creating new directory structure..." "Yellow"
    
    $directories = @(
        # Core framework
        "src/core",
        "src/core/utility",
        "src/core/sdk",
        "src/core/sdk/threading",
        "src/core/sdk/math",
        
        # Adapters
        "src/adapters/common",
        "src/adapters/mt-framework",
        "src/adapters/re-engine", 
        "src/adapters/redengine4",
        
        # Hooks
        "src/hooks",
        "src/hooks/d3d11",
        "src/hooks/d3d12",
        "src/hooks/opengl",
        "src/hooks/vulkan",
        
        # Mods
        "src/mods",
        "src/mods/vr",
        "src/mods/vr/d3d11",
        "src/mods/vr/d3d12",
        "src/mods/pluginloader",
        "src/mods/uobjecthook",
        
        # Utilities
        "src/utils",
        "src/utils/rendering",
        "src/utils/scripting",
        
        # Tests
        "tests/unit",
        "tests/integration",
        "tests/fixtures",
        "tests/performance",
        
        # Examples
        "examples/basic_plugin",
        "examples/custom_adapter",
        "examples/lua_scripting",
        "examples/vr_mod",
        
        # Documentation
        "docs/api",
        "docs/guides",
        "docs/examples",
        
        # Build system
        "cmake",
        "scripts",
        "tools"
    )
    
    $createdCount = 0
    foreach ($dir in $directories) {
        if (-not (Test-Path $dir)) {
            try {
                New-Item -ItemType Directory -Name $dir -Force | Out-Null
                Write-ColorOutput "Created: $dir" "Cyan"
                $createdCount++
            } catch {
                Write-ColorOutput "Failed to create: $dir - $_" "Red"
            }
        } else {
            Write-ColorOutput "Exists: $dir" "Gray"
        }
    }
    
    Write-ColorOutput "Directory structure created: $createdCount new directories" "Green"
    return $directories
}

# Function to migrate source files
function Migrate-SourceFiles {
    Write-ColorOutput "Migrating source files..." "Yellow"
    
    $migrationMap = @{
        # Core framework files
        "SOURCECODE/uevr/src/Framework.cpp" = "src/core/"
        "SOURCECODE/uevr/src/Framework.hpp" = "include/uevr/"
        "SOURCECODE/uevr/src/ExceptionHandler.cpp" = "src/core/"
        "SOURCECODE/uevr/src/ExceptionHandler.hpp" = "src/core/"
        "SOURCECODE/uevr/src/Plugin.cpp" = "src/"
        "SOURCECODE/uevr/src/Plugin.hpp" = "include/uevr/"
        
        # Adapter files
        "adapters/MT-Framework" = "src/adapters/mt-framework"
        "adapters/RE-Engine" = "src/adapters/re-engine"
        "adapters/REDengine4" = "src/adapters/redengine4"
        "adapters/common" = "src/adapters/common"
        
        # Hook files
        "SOURCECODE/uevr/src/hooks" = "src/hooks"
        
        # Mod files
        "SOURCECODE/uevr/src/mods" = "src/mods"
        
        # Utility files
        "SOURCECODE/uevr/src/utility" = "src/core/utility"
        "SOURCECODE/uevr/src/sdk" = "src/core/sdk"
    }
    
    $migratedCount = 0
    foreach ($source in $migrationMap.Keys) {
        $destination = $migrationMap[$source]
        
        if (Test-Path $source) {
            try {
                if (Test-Path $destination) {
                    if ($Force) {
                        Remove-Item $destination -Recurse -Force
                    } else {
                        Write-ColorOutput "Destination exists, skipping: $destination" "Yellow"
                        continue
                    }
                }
                
                Copy-Item -Path $source -Destination $destination -Recurse -Force
                Write-ColorOutput "Migrated: $source -> $destination" "Green"
                $migratedCount++
            } catch {
                Write-ColorOutput "Failed to migrate: $source - $_" "Red"
            }
        } else {
            Write-ColorOutput "Source not found: $source" "Gray"
        }
    }
    
    Write-ColorOutput "File migration completed: $migratedCount files migrated" "Green"
}

# Function to update include paths
function Update-IncludePaths {
    Write-ColorOutput "Updating include paths..." "Yellow"
    
    $includeMappings = @{
        '\.\.\/\.\.\/include\/uevr\/' = 'uevr/'
        '\.\.\/common\/' = 'uevr/adapters/common/'
        '\.\.\/\.\.\/SOURCECODE\/uevr\/src\/' = 'uevr/core/'
        '\.\.\/\.\.\/SOURCECODE\/uevr\/include\/' = 'uevr/'
        '\.\.\/\.\.\/adapters\/' = 'uevr/adapters/'
        '\.\.\/\.\.\/hooks\/' = 'uevr/hooks/'
        '\.\.\/\.\.\/mods\/' = 'uevr/mods/'
    }
    
    $sourceFiles = Get-ChildItem -Path "src" -Recurse -Include "*.cpp", "*.hpp", "*.h", "*.c"
    $updatedCount = 0
    
    foreach ($file in $sourceFiles) {
        $content = Get-Content $file.FullName -Raw
        $originalContent = $content
        
        foreach ($pattern in $includeMappings.Keys) {
            $replacement = $includeMappings[$pattern]
            $content = $content -replace $pattern, $replacement
        }
        
        if ($content -ne $originalContent) {
            try {
                Set-Content -Path $file.FullName -Value $content -NoNewline
                Write-ColorOutput "Updated: $($file.FullName)" "Cyan"
                $updatedCount++
            } catch {
                Write-ColorOutput "Failed to update: $($file.FullName) - $_" "Red"
            }
        }
    }
    
    Write-ColorOutput "Include paths updated: $updatedCount files modified" "Green"
}

# Function to validate migration
function Test-Migration {
    Write-ColorOutput "Validating migration..." "Yellow"
    
    $validationChecks = @(
        @{ Path = "src/core"; Description = "Core framework" },
        @{ Path = "src/adapters"; Description = "Adapters" },
        @{ Path = "src/hooks"; Description = "Hooks" },
        @{ Path = "src/mods"; Description = "Mods" },
        @{ Path = "include/uevr"; Description = "Public headers" },
        @{ Path = "tests"; Description = "Test framework" },
        @{ Path = "examples"; Description = "Examples" }
    )
    
    $validationResults = @()
    foreach ($check in $validationChecks) {
        $exists = Test-Path $check.Path
        $status = if ($exists) { "✓" } else { "✗" }
        $color = if ($exists) { "Green" } else { "Red" }
        
        Write-ColorOutput "$status $($check.Description): $($check.Path)" $color
        $validationResults += @{ Path = $check.Path; Exists = $exists }
    }
    
    $successCount = ($validationResults | Where-Object { $_.Exists }).Count
    $totalCount = $validationResults.Count
    
    Write-ColorOutput "Validation completed: $successCount/$totalCount checks passed" "Green"
    
    return $successCount -eq $totalCount
}

# Main execution
try {
    Write-Host ""
    Write-ColorOutput "========================================" "Magenta"
    Write-ColorOutput "    uevr Project Structure Migration" "Magenta"
    Write-ColorOutput "========================================" "Magenta"
    Write-Host ""
    
    # Create backup if requested
    if ($Backup) {
        $backupDir = "backup_$(Get-Timestamp)"
        Backup-OriginalFiles -BackupDir $backupDir
        Write-Host ""
    }
    
    # Create directory structure
    $directories = New-DirectoryStructure
    Write-Host ""
    
    # Migrate source files
    Migrate-SourceFiles
    Write-Host ""
    
    # Update include paths
    Update-IncludePaths
    Write-Host ""
    
    # Validate migration
    $validationSuccess = Test-Migration
    Write-Host ""
    
    if ($validationSuccess) {
        Write-ColorOutput "========================================" "Green"
        Write-ColorOutput "Migration completed successfully!" "Green"
        Write-ColorOutput "========================================" "Green"
        Write-Host ""
        
        Write-ColorOutput "Next steps:" "Yellow"
        Write-ColorOutput "1. Review migrated files for any issues" "White"
        Write-ColorOutput "2. Run build_restructured.ps1 to test the new structure" "White"
        Write-ColorOutput "3. Run tests to ensure functionality is preserved" "White"
        Write-ColorOutput "4. Update documentation if needed" "White"
        Write-Host ""
    } else {
        Write-ColorOutput "========================================" "Red"
        Write-ColorOutput "Migration completed with issues!" "Red"
        Write-ColorOutput "========================================" "Red"
        Write-Host ""
        
        Write-ColorOutput "Please review the validation results above and fix any issues." "Yellow"
        Write-ColorOutput "You may need to manually migrate some files." "Yellow"
        Write-Host ""
    }
    
} catch {
    Write-ColorOutput "========================================" "Red"
    Write-ColorOutput "Migration failed!" "Red"
    Write-ColorOutput "========================================" "Red"
    Write-Host ""
    Write-ColorOutput "Error: $_" "Red"
    Write-Host ""
    exit 1
}

Write-ColorOutput "========================================" "Cyan"
Write-ColorOutput "Migration process completed!" "Cyan"
Write-ColorOutput "========================================" "Cyan"
Write-Host ""

Read-Host "Press Enter to continue..."
