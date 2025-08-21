#!/bin/bash

# uevrLCX Build Script for Linux/macOS
# =====================================

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show help
show_help() {
    echo "uevrLCX Build Script for Linux/macOS"
    echo
    echo "Usage: ./build.sh [options]"
    echo
    echo "Options:"
    echo "  --debug     Build in Debug mode"
    echo "  --release   Build in Release mode (default)"
    echo "  --clean     Clean build directory before building"
    echo "  --test      Run tests after building"
    echo "  --package   Create installer package"
    echo "  --verbose   Show detailed build output"
    echo "  --help      Show this help message"
    echo
    echo "Examples:"
    echo "  ./build.sh                    # Build in Release mode"
    echo "  ./build.sh --debug            # Build in Debug mode"
    echo "  ./build.sh --clean --test     # Clean build and run tests"
    echo "  ./build.sh --package          # Build and create installer"
    echo
}

# Set default values
BUILD_TYPE="Release"
BUILD_DIR="build"
CLEAN_BUILD=false
RUN_TESTS=false
PACKAGE=false
VERBOSE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --test)
            RUN_TESTS=true
            shift
            ;;
        --package)
            PACKAGE=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Print banner
echo "========================================"
echo "    uevrLCX Build Script for Linux/macOS"
echo "========================================"
echo

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed or not in PATH"
    echo "Please install CMake:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  CentOS/RHEL: sudo yum install cmake"
    echo "  macOS: brew install cmake"
    echo "  Or download from: https://cmake.org/download/"
    exit 1
fi

# Check if a C++ compiler is available
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    print_error "No C++ compiler found"
    echo "Please install a C++ compiler:"
    echo "  Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  CentOS/RHEL: sudo yum groupinstall 'Development Tools'"
    echo "  macOS: Install Xcode Command Line Tools"
    exit 1
fi

# Detect compiler
if command -v clang++ &> /dev/null; then
    COMPILER="clang++"
    print_status "Using Clang++ compiler"
elif command -v g++ &> /dev/null; then
    COMPILER="g++"
    print_status "Using G++ compiler"
fi

# Check for required packages
print_status "Checking for required packages..."

# Check for OpenGL development libraries
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    if ! pkg-config --exists gl; then
        print_warning "OpenGL development libraries not found"
        echo "Install with: sudo apt-get install libgl1-mesa-dev (Ubuntu/Debian)"
        echo "              sudo yum install mesa-libGL-devel (CentOS/RHEL)"
    fi
    
    if ! pkg-config --exists x11; then
        print_warning "X11 development libraries not found"
        echo "Install with: sudo apt-get install libx11-dev (Ubuntu/Debian)"
        echo "              sudo yum install libX11-devel (CentOS/RHEL)"
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    print_status "macOS detected - OpenGL libraries should be available"
fi

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_status "Cleaning build directory..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
    echo
fi

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

# Configure CMake
print_status "Configuring CMake for $BUILD_TYPE build..."
cd "$BUILD_DIR"

CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
if [ "$VERBOSE" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON"
fi

# Set compiler if specified
if [ -n "$COMPILER" ]; then
    if [ "$COMPILER" = "clang++" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_COMPILER=clang++"
    elif [ "$COMPILER" = "g++" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_COMPILER=g++"
    fi
fi

if [ "$VERBOSE" = true ]; then
    cmake .. $CMAKE_ARGS
else
    cmake .. $CMAKE_ARGS > /dev/null
fi

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed"
    cd ..
    exit 1
fi

# Build the project
echo
print_status "Building uevrLCX..."

if [ "$VERBOSE" = true ]; then
    cmake --build . --config "$BUILD_TYPE" --verbose
else
    cmake --build . --config "$BUILD_TYPE"
fi

if [ $? -ne 0 ]; then
    print_error "Build failed"
    cd ..
    exit 1
fi

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    echo
    print_status "Running tests..."
    if command -v ctest &> /dev/null; then
        ctest --output-on-failure
        if [ $? -ne 0 ]; then
            print_warning "Some tests failed"
        fi
    else
        print_warning "CTest not found, skipping tests"
    fi
fi

# Create package if requested
if [ "$PACKAGE" = true ]; then
    echo
    print_status "Creating package..."
    cmake --build . --target package
    if [ $? -ne 0 ]; then
        print_warning "Package creation failed"
    fi
fi

# Return to original directory
cd ..

# Show build results
echo
echo "========================================"
echo "           Build Complete!"
echo "========================================"
echo "Build Type: $BUILD_TYPE"
echo "Build Directory: $BUILD_DIR"
echo

# Check if executable was created
if [ -f "$BUILD_DIR/bin/uevrLCX" ]; then
    print_success "uevrLCX executable created successfully"
    echo "Location: $BUILD_DIR/bin/uevrLCX"
    echo
    echo "To run uevrLCX:"
    echo "  $BUILD_DIR/bin/uevrLCX --help"
    
    # Make executable
    chmod +x "$BUILD_DIR/bin/uevrLCX"
else
    print_error "uevrLCX executable not found"
    echo "Check build output for errors"
fi

# Show available targets
echo
echo "Available build targets:"
echo "  --debug     Build in Debug mode"
echo "  --release   Build in Release mode (default)"
echo "  --clean     Clean build directory before building"
echo "  --test      Run tests after building"
echo "  --package   Create installer package"
echo "  --verbose   Show detailed build output"
echo "  --help      Show this help message"

print_success "Build completed successfully!"
