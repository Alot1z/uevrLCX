# uevr Project Migration Guide

## Overview

This guide provides step-by-step instructions for migrating the uevr project from its current structure to the new, improved organization. The migration will improve code organization, build system modularity, and overall maintainability.

## Prerequisites

- PowerShell (for running migration scripts)
- CMake 3.16 or later
- Visual Studio 2019 or later (for Windows builds)
- Git (for version control)

## Migration Steps

### Step 1: Backup Your Project

Before starting the migration, create a backup:

```bash
# Create a backup branch
git checkout -b backup-before-restructure

# Or create a complete copy of the project
cp -r uevr uevr_backup
```

### Step 2: Run the Migration Script

Execute the migration script to create the new directory structure:

```powershell
.\migrate_to_new_structure.ps1
```

This will create all necessary directories for the new structure.

### Step 3: Move Source Files

Manually move files from old locations to new ones:

#### Core Files
```
SOURCECODE/uevr/src/Framework.cpp → src/core/
SOURCECODE/uevr/src/ExceptionHandler.cpp → src/core/
SOURCECODE/uevr/src/utility/ → src/core/utility/
```

#### Adapter Files
```
adapters/MT-Framework/ → src/adapters/mt-framework/
adapters/RE-Engine/ → src/adapters/re-engine/
adapters/REDengine4/ → src/adapters/redengine4/
adapters/common/ → src/adapters/common/
```

#### Hook and Mod Files
```
SOURCECODE/uevr/src/hooks/ → src/hooks/
SOURCECODE/uevr/src/mods/ → src/mods/
```

#### Test Files
```
adapters/MT-Framework/test_integration.cpp → tests/integration/
```

#### Example Files
```
SOURCECODE/uevr/examples/ → examples/
```

### Step 4: Update Include Paths

Run the include path update script:

```powershell
.\update_include_paths.ps1
```

This script will automatically update most include statements. However, you should manually review the changes to ensure accuracy.

#### Common Include Path Changes

**Old:**
```cpp
#include "../../include/uevr/CrossEngineAdapterRegistry.hpp"
#include "../common/matrix_extraction.hpp"
#include "../../SOURCECODE/uevr/src/Framework.hpp"
```

**New:**
```cpp
#include "uevr/CrossEngineAdapterRegistry.hpp"
#include "uevr/adapters/common/matrix_extraction.hpp"
#include "uevr/core/Framework.hpp"
```

### Step 5: Update CMake Files

The new CMake configuration files have been created. You may need to adjust them based on your specific file locations and dependencies.

#### Key CMake Changes

1. **Modular Structure**: Each component has its own CMakeLists.txt
2. **Dependency Management**: Clear dependency relationships
3. **Build Options**: Easy to enable/disable components
4. **Output Organization**: Consistent output directory structure

### Step 6: Test the Build

Test the new build system:

```powershell
.\build_restructured.ps1
```

If there are build errors, address them one by one:

1. **Include Path Errors**: Check that all includes use the new paths
2. **Missing Files**: Ensure all source files are in the correct locations
3. **Dependency Issues**: Verify that all dependencies are properly linked

### Step 7: Update Documentation

Update any documentation that references old file paths:

1. **README files**: Update installation and build instructions
2. **API documentation**: Update include examples
3. **Development guides**: Update file organization information

### Step 8: Test Functionality

After successful compilation, test that the functionality still works:

1. **Run unit tests**: Ensure all tests pass
2. **Test adapters**: Verify adapter functionality
3. **Check examples**: Ensure examples compile and run
4. **Integration testing**: Test the complete system

## File Organization Rules

### Source Files (`src/`)

- **Core**: Main framework, exception handling, utilities
- **Adapters**: Engine-specific adapter implementations
- **Hooks**: Engine hooking and interception code
- **Mods**: VR modification and enhancement code

### Headers (`include/`)

- **Public API**: Headers that external code should include
- **Internal**: Headers only used within the project
- **Third-party**: External library headers

### Tests (`tests/`)

- **Unit**: Individual component tests
- **Integration**: Cross-component integration tests
- **Fixtures**: Test data and setup code

### Examples (`examples/`)

- **Basic**: Simple usage examples
- **Advanced**: Complex implementation examples
- **Tutorials**: Step-by-step learning examples

## Common Issues and Solutions

### Issue: Include Path Not Found

**Symptoms**: Compilation errors about missing headers

**Solution**: 
1. Check that the include path is correct
2. Verify the header file exists in the expected location
3. Update the include statement to use the new path

### Issue: Missing Source Files

**Symptoms**: Linker errors about missing symbols

**Solution**:
1. Ensure all source files are in the correct directories
2. Check that CMakeLists.txt includes all necessary files
3. Verify file paths in the build configuration

### Issue: Dependency Linking Errors

**Symptoms**: Runtime errors or missing DLLs

**Solution**:
1. Check that all dependencies are properly linked
2. Verify library paths in CMake configuration
3. Ensure runtime dependencies are available

### Issue: Test Failures

**Symptoms**: Tests fail after migration

**Solution**:
1. Check that test files are in the correct locations
2. Verify that test dependencies are properly linked
3. Update test include paths if necessary

## Post-Migration Tasks

### 1. Clean Up Old Structure

After successful migration, remove old directories:

```bash
# Remove old adapter directories
rm -rf adapters/

# Remove old source directories
rm -rf SOURCECODE/uevr/src/

# Remove old build files
rm -rf build/
```

### 2. Update CI/CD

If you have continuous integration:

1. Update build scripts to use new structure
2. Modify test runners to use new test locations
3. Update deployment scripts if necessary

### 3. Update Development Workflow

1. **IDE Configuration**: Update project files for new structure
2. **Debugging**: Ensure debug symbols point to correct locations
3. **Profiling**: Update profiling tools configuration

### 4. Team Communication

1. **Documentation**: Update team documentation
2. **Training**: Train team members on new structure
3. **Standards**: Establish coding standards for new organization

## Verification Checklist

- [ ] All source files moved to new locations
- [ ] Include paths updated throughout codebase
- [ ] CMake configuration files updated
- [ ] Project builds successfully
- [ ] All tests pass
- [ ] Examples compile and run
- [ ] Documentation updated
- [ ] Team informed of changes
- [ ] Old structure cleaned up
- [ ] Version control updated

## Rollback Plan

If issues arise during migration:

1. **Immediate Rollback**: Use git to revert to backup branch
2. **Partial Rollback**: Restore specific components to old structure
3. **Incremental Migration**: Migrate one component at a time

## Support

If you encounter issues during migration:

1. Check this guide for common solutions
2. Review the error messages carefully
3. Check that all prerequisites are met
4. Verify file locations and paths
5. Test with a minimal configuration first

## Conclusion

The new project structure provides significant improvements in organization, maintainability, and scalability. While the migration requires effort, the long-term benefits make it worthwhile. Take your time, test thoroughly, and don't hesitate to rollback if needed.

The modular structure will make future development much easier and provide a solid foundation for uevr's continued growth.
