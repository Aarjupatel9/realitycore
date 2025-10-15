#!/bin/bash

# Enhanced Cross-Platform RealityCore Physics Engine Build Script
# Features: Smart incremental builds, dependency caching, selective cleaning
# Compatible with macOS, Linux, and Windows (via Git Bash/MSYS2)

set -e  # Exit on any error
set -o pipefail  # Ensure pipelines fail if any command fails

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# Build configuration
BUILD_MODE="auto"           # auto, soft, hard, deps-only, project-only
BUILD_CONFIG="Release"      # Release, Debug
BUILD_TARGET=""             # Specific target to build
SHOW_TIMING=false
SHOW_HELP=false
VERBOSE=false
TESTS_ONLY=false
RUN_TESTS=false

# Build info file
BUILD_INFO_FILE="build/.build_info"

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

print_timing() {
    if [ "$SHOW_TIMING" = true ]; then
        echo -e "${CYAN}[TIMING]${NC} $1"
    fi
}

print_verbose() {
    if [ "$VERBOSE" = true ]; then
        echo -e "${PURPLE}[VERBOSE]${NC} $1"
    fi
}

# Function to show help
show_help() {
    echo "Enhanced RealityCore Physics Engine Build Script"
    echo ""
    echo "USAGE:"
    echo "  ./build.sh [OPTIONS]"
    echo ""
    echo "BUILD MODES:"
    echo "  (default)           Auto-detect best build mode (smart incremental)"
    echo "  --soft              Incremental build (skip external deps if exist)"
    echo "  --hard              Full clean rebuild everything"
    echo "  --deps-only         Build only external dependencies"
    echo "  --project-only      Build only project files (assumes deps exist)"
    echo ""
    echo "CLEANING OPTIONS:"
    echo "  --clean-deps        Clean external dependencies cache"
    echo "  --clean-project     Clean project build files only"
    echo "  --clean-all         Clean everything (same as --hard)"
    echo ""
    echo "BUILD OPTIONS:"
    echo "  --target <name>     Build specific demo only (e.g., BallCollision2)"
    echo "  --tests-only        Build unit tests target only"
    echo "  --run-tests         Run tests after building (implies tests are enabled)"
    echo "  --debug             Debug build with symbols"
    echo "  --release           Release build (default)"
    echo ""
    echo "OTHER OPTIONS:"
    echo "  --timing            Show detailed build timing"
    echo "  --verbose           Show verbose output"
    echo "  --help              Show this help message"
    echo ""
    echo "EXAMPLES:"
    echo "  ./build.sh                          # Smart incremental build"
    echo "  ./build.sh --soft --timing          # Fast incremental with timing"
    echo "  ./build.sh --hard                   # Full rebuild everything"
    echo "  ./build.sh --target BallCollision2  # Build specific demo"
    echo "  ./build.sh --deps-only              # Build dependencies only"
    echo "  ./build.sh --project-only --debug   # Quick project build (debug)"
    echo "  ./build.sh --tests-only               # Build tests target only"
    echo "  ./build.sh --tests-only --run-tests   # Build and run tests"
    echo ""
}

# Parse command line arguments
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --soft)
                BUILD_MODE="soft"
                shift
                ;;
            --hard)
                BUILD_MODE="hard"
                shift
                ;;
            --deps-only)
                BUILD_MODE="deps-only"
                shift
                ;;
            --project-only)
                BUILD_MODE="project-only"
                shift
                ;;
            --clean-deps)
                BUILD_MODE="clean-deps"
                shift
                ;;
            --clean-project)
                BUILD_MODE="clean-project"
                shift
                ;;
            --clean-all)
                BUILD_MODE="hard"
                shift
                ;;
            --target)
                BUILD_TARGET="$2"
                shift 2
                ;;
            --debug)
                BUILD_CONFIG="Debug"
                shift
                ;;
            --release)
                BUILD_CONFIG="Release"
                shift
                ;;
            --timing)
                SHOW_TIMING=true
                shift
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --tests-only)
                TESTS_ONLY=true
                shift
                ;;
            --run-tests)
                RUN_TESTS=true
                shift
                ;;
            --help)
                SHOW_HELP=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
}

# Detect operating system
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        OS="windows"
    else
        OS="unknown"
    fi
    print_verbose "Detected OS: $OS"
}

# Function to get timestamp
get_timestamp() {
    date +%s
}

# Function to calculate duration
calculate_duration() {
    local start_time=$1
    local end_time=$2
    local duration=$((end_time - start_time))
    
    if [ $duration -lt 60 ]; then
        echo "${duration}s"
    else
        local minutes=$((duration / 60))
        local seconds=$((duration % 60))
        echo "${minutes}m ${seconds}s"
    fi
}

# Check if dependencies exist and are up to date
check_dependencies() {
    print_verbose "Checking dependency cache..."
    
    local deps_exist=true
    local deps_dirs=("build/deps/bullet3-build" "build/deps/glfw-build" "build/deps/glad-build" "build/deps/glm-build")
    
    for dep_dir in "${deps_dirs[@]}"; do
        if [ ! -d "$dep_dir" ]; then
            print_verbose "Missing dependency: $dep_dir"
            deps_exist=false
            break
        fi
    done
    
    if [ "$deps_exist" = true ]; then
        print_status "External dependencies found in cache"
        return 0
    else
        print_status "External dependencies need to be built"
        return 1
    fi
}

# Save build info
save_build_info() {
    local timestamp=$(get_timestamp)
    local commit_hash=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
    
    mkdir -p "$(dirname "$BUILD_INFO_FILE")"
    cat > "$BUILD_INFO_FILE" << EOF
BUILD_TIMESTAMP=$timestamp
BUILD_MODE=$BUILD_MODE
BUILD_CONFIG=$BUILD_CONFIG
COMMIT_HASH=$commit_hash
OS=$OS
EOF
    print_verbose "Saved build info to $BUILD_INFO_FILE"
}

# Load build info
load_build_info() {
    if [ -f "$BUILD_INFO_FILE" ]; then
        source "$BUILD_INFO_FILE"
        print_verbose "Loaded previous build info"
        return 0
    else
        print_verbose "No previous build info found"
        return 1
    fi
}

# Set environment variables for better Git performance during build
configure_git_for_large_repos() {
    print_verbose "Setting Git environment variables for large repository downloads..."
    
    # Set environment variables for this build session only (no global changes)
    export GIT_HTTP_MAX_REQUEST_BUFFER=100M
    export GIT_HTTP_POST_BUFFER=524288000
    export GIT_HTTP_LOW_SPEED_LIMIT=0
    export GIT_HTTP_LOW_SPEED_TIME=999999
    
    print_verbose "Git environment configured for build session"
}

# Progress monitoring functions
show_download_progress() {
    local dep_name="$1"
    local dep_size="$2"
    local start_time=$(get_timestamp)
    
    print_status "📥 Downloading $dep_name (~${dep_size}MB)..."
    
    # Show progress dots while download is happening
    while true; do
        sleep 2
        local elapsed=$(($(get_timestamp) - start_time))
        
        # Show progress every 15 seconds for the first 2 minutes, then every 30 seconds
        if [ $elapsed -le 120 ] && [ $((elapsed % 15)) -eq 0 ] && [ $elapsed -gt 0 ]; then
            print_verbose "⏳ $dep_name download in progress... (${elapsed}s elapsed)"
        elif [ $elapsed -gt 120 ] && [ $((elapsed % 30)) -eq 0 ]; then
            print_verbose "⏳ $dep_name download in progress... (${elapsed}s elapsed)"
        fi
        
        # Check if download completed by looking for source directory
        if [ -d "deps/${dep_name}-src" ]; then
            local total_time=$(($(get_timestamp) - start_time))
            print_success "✅ $dep_name downloaded successfully (${total_time}s)"
            break
        fi
        
        # Timeout after 10 minutes
        if [ $elapsed -gt 600 ]; then
            print_error "❌ $dep_name download timed out after ${elapsed}s"
            break
        fi
    done
}

show_build_progress() {
    local target="$1"
    local start_time=$(get_timestamp)
    
    print_status "🔨 Building $target..."
    
    # Monitor build progress by checking for completion
    while true; do
        sleep 5
        local elapsed=$(($(get_timestamp) - start_time))
        
        # Check if build completed - look for various possible output locations
        local completed=false
        if [ -f "deps/${target}-build/lib/lib${target}.a" ] || \
           [ -f "deps/${target}-build/lib/${target}.dylib" ] || \
           [ -f "deps/${target}-build/lib/${target}.so" ] || \
           [ -f "deps/${target}-build/${target}.dylib" ] || \
           [ -f "deps/${target}-build/${target}.so" ] || \
           [ -d "deps/${target}-build/include" ] || \
           [ -f "deps/${target}-build/CMakeCache.txt" ]; then
            completed=true
        fi
        
        if [ "$completed" = true ]; then
            local total_time=$(($(get_timestamp) - start_time))
            print_success "✅ $target built successfully (${total_time}s)"
            break
        fi
        
        # Show progress every 30 seconds
        if [ $((elapsed % 30)) -eq 0 ] && [ $elapsed -gt 0 ]; then
            print_verbose "⏳ $target build in progress... (${elapsed}s elapsed)"
        fi
        
        # Timeout after 15 minutes
        if [ $elapsed -gt 900 ]; then
            print_error "❌ $target build timed out after ${elapsed}s"
            break
        fi
    done
}

# Enhanced dependency download with progress monitoring
download_dependency_with_progress() {
    local dep_name="$1"
    local dep_url="$2"
    local dep_size="$3"
    
    # Start progress monitoring in background
    show_download_progress "$dep_name" "$dep_size" &
    local progress_pid=$!
    
    # Perform the actual download
    local download_success=false
    if git clone --depth 1 "$dep_url" "deps/${dep_name}-src"; then
        download_success=true
    fi
    
    # Stop progress monitoring
    kill $progress_pid 2>/dev/null || true
    wait $progress_pid 2>/dev/null || true
    
    if [ "$download_success" = true ]; then
        return 0
    else
        return 1
    fi
}

# Check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."
    
    local missing_tools=()
    
    # Check CMake
    if ! command -v cmake &> /dev/null; then
        missing_tools+=("cmake")
    else
        local cmake_version=$(cmake --version | head -n1 | cut -d' ' -f3)
        print_verbose "CMake version: $cmake_version"
    fi
    
    # Check Git
    if ! command -v git &> /dev/null; then
        missing_tools+=("git")
    else
        local git_version=$(git --version)
        print_verbose "Git found: $git_version"
    fi
    
    # Check C++ compiler
    if [[ "$OS" == "windows" ]]; then
        if ! command -v cl &> /dev/null && ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
            missing_tools+=("c++ compiler (cl.exe, g++, or clang++)")
        fi
    else
        if command -v clang++ &> /dev/null; then
            local compiler_version=$(clang++ --version | head -n1)
            print_verbose "Clang++ found: $compiler_version"
        elif command -v g++ &> /dev/null; then
            local compiler_version=$(g++ --version | head -n1)
            print_verbose "G++ found: $compiler_version"
        else
            missing_tools+=("c++ compiler (g++ or clang++)")
        fi
    fi
    
    if [ ${#missing_tools[@]} -gt 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        show_installation_help
        exit 1
    fi
    
    print_success "All prerequisites found!"
}

# Installation help
show_installation_help() {
    echo ""
    print_warning "Installation instructions:"
    
    if [[ "$OS" == "macos" ]]; then
        echo "  macOS:"
        echo "    1. Install Xcode Command Line Tools: xcode-select --install"
        echo "    2. Install Homebrew: /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
        echo "    3. Install CMake: brew install cmake"
    elif [[ "$OS" == "linux" ]]; then
        echo "  Ubuntu/Debian:"
        echo "    sudo apt update && sudo apt install build-essential cmake git"
        echo "  CentOS/RHEL:"
        echo "    sudo yum groupinstall \"Development Tools\" && sudo yum install cmake git"
        echo "  Arch Linux:"
        echo "    sudo pacman -S base-devel cmake git"
    elif [[ "$OS" == "windows" ]]; then
        echo "  Windows (MSYS2/MinGW):"
        echo "    1. Install MSYS2 from https://www.msys2.org/"
        echo "    2. Open MSYS2 terminal and run:"
        echo "    3. pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake git"
    fi
}

# Clean functions
clean_dependencies() {
    print_status "Cleaning external dependencies..."
    if [ -d "build/deps" ]; then
        rm -rf build/deps/*
        print_success "External dependencies cleaned"
    else
        print_status "No dependencies to clean"
    fi
}

clean_project() {
    print_status "Cleaning project build files..."
    local dirs_to_clean=("build/engine" "build/demos" "build/bin" "build/lib")
    
    for dir in "${dirs_to_clean[@]}"; do
        if [ -d "$dir" ]; then
            rm -rf "$dir"
            print_verbose "Cleaned: $dir"
        fi
    done
    
    # Remove build info
    if [ -f "$BUILD_INFO_FILE" ]; then
        rm "$BUILD_INFO_FILE"
        print_verbose "Removed build info file"
    fi
    
    print_success "Project build files cleaned"
}

clean_all() {
    print_status "Cleaning entire build directory..."
    if [ -d "build" ]; then
        rm -rf build/*
        print_success "Entire build directory cleaned"
    else
        print_status "No build directory to clean"
    fi
}

# Determine optimal CPU count
get_cpu_count() {
    if command -v nproc &> /dev/null; then
        nproc
    elif command -v sysctl &> /dev/null; then
        sysctl -n hw.ncpu
    else
        echo 4
    fi
}

# Build external dependencies with retry mechanism and progress monitoring
build_dependencies() {
    print_status "Building external dependencies..."
    local start_time=$(get_timestamp)
    
    # Show overall dependency build progress
    print_status "🚀 Starting dependency build process..."
    print_verbose "Dependencies to build: Bullet Physics (~200MB), GLFW (~50MB), GLM (~30MB), GLAD (~20MB)"
    print_verbose "Estimated total time: 5-15 minutes depending on network and CPU speed"
    
    cd build
    
    # Configure with CMake with retry mechanism
    print_status "Configuring dependencies with CMake..."
    local max_retries=3
    local retry_count=0
    local cmake_success=false
    
    while [ $retry_count -lt $max_retries ] && [ "$cmake_success" = false ]; do
        if [ $retry_count -gt 0 ]; then
            print_warning "Retrying CMake configuration (attempt $((retry_count + 1))/$max_retries)..."
            sleep 5  # Wait 5 seconds before retry
        fi
        
        # Clean any partial downloads and corrupted git repositories
        if [ -d "deps" ]; then
            print_status "Cleaning partial dependency downloads..."
            # Remove all dependency source directories
            find deps -name "*-src" -type d -exec rm -rf {} + 2>/dev/null || true
            # Remove any temporary files
            find deps -name "*.tmp" -delete 2>/dev/null || true
            # Remove any corrupted git repositories
            find deps -name ".git" -type d -exec rm -rf {} + 2>/dev/null || true
            # Remove the entire deps directory to start fresh
            rm -rf deps 2>/dev/null || true
        fi
        
        # Also clean the build directory's CMake cache
        if [ -f "CMakeCache.txt" ]; then
            print_status "Cleaning CMake cache..."
            rm -f CMakeCache.txt 2>/dev/null || true
            rm -rf CMakeFiles/ 2>/dev/null || true
        fi
        
        if [[ "$OS" == "windows" ]]; then
            if command -v cl &> /dev/null; then
                if cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_POLICY_VERSION_MINIMUM=3.5; then
                    cmake_success=true
                fi
            else
                if cmake .. -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5; then
                    cmake_success=true
                fi
            fi
        else
            if cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5; then
                cmake_success=true
            fi
        fi
        
        retry_count=$((retry_count + 1))
    done
    
    if [ "$cmake_success" = false ]; then
        print_error "Failed to configure dependencies after $max_retries attempts"
        print_error "This is likely due to network connectivity issues with GitHub"
        print_warning "Possible solutions:"
        echo "  1. Check your internet connection"
        echo "  2. Try again later (GitHub may be experiencing issues)"
        echo "  3. Use a VPN if you're behind a restrictive firewall"
        echo "  4. Try using a different network connection (mobile hotspot, etc.)"
        cd ..
        exit 1
    fi
    
    # Build only dependencies (not our project)
    print_status "Building dependencies..."
    local cpu_count=$(get_cpu_count)
    print_verbose "Using $cpu_count parallel jobs"
    
    if [[ "$OS" == "windows" ]] && command -v cl &> /dev/null; then
        # Build dependencies with MSBuild and progress monitoring
        print_status "Building dependencies with progress monitoring..."
        
        # Start build progress monitoring for each dependency
        show_build_progress "bullet3" &
        local bullet_progress_pid=$!
        
        show_build_progress "glfw" &
        local glfw_progress_pid=$!
        
        show_build_progress "glm" &
        local glm_progress_pid=$!
        
        show_build_progress "glad" &
        local glad_progress_pid=$!
        
        # Perform the actual build
        local build_success=false
        if cmake --build . --config $BUILD_CONFIG --target BulletDynamics BulletCollision BulletSoftBody LinearMath glfw glad -- -maxcpucount:$cpu_count; then
            build_success=true
        fi
        
        # Stop all progress monitoring
        kill $bullet_progress_pid $glfw_progress_pid $glm_progress_pid $glad_progress_pid 2>/dev/null || true
        wait $bullet_progress_pid $glfw_progress_pid $glm_progress_pid $glad_progress_pid 2>/dev/null || true
        
        if [ "$build_success" = false ]; then
            print_error "Failed to build dependencies"
            cd ..
            exit 1
        fi
    else
        # Build dependencies with make and progress monitoring
        print_status "Building dependencies with progress monitoring..."
        
        # Start build progress monitoring for each dependency
        show_build_progress "bullet3" &
        local bullet_progress_pid=$!
        
        show_build_progress "glfw" &
        local glfw_progress_pid=$!
        
        show_build_progress "glm" &
        local glm_progress_pid=$!
        
        show_build_progress "glad" &
        local glad_progress_pid=$!
        
        # Perform the actual build
        local build_success=false
        if make -j$cpu_count BulletDynamics BulletCollision BulletSoftBody LinearMath glfw glad; then
            build_success=true
        fi
        
        # Stop all progress monitoring
        kill $bullet_progress_pid $glfw_progress_pid $glm_progress_pid $glad_progress_pid 2>/dev/null || true
        wait $bullet_progress_pid $glfw_progress_pid $glm_progress_pid $glad_progress_pid 2>/dev/null || true
        
        if [ "$build_success" = false ]; then
            print_error "Failed to build dependencies"
            cd ..
            exit 1
        fi
    fi
    
    cd ..
    
    local end_time=$(get_timestamp)
    local duration=$(calculate_duration $start_time $end_time)
    print_timing "Dependencies built in: $duration"
    print_success "External dependencies built successfully!"
}

# Build project
build_project() {
    print_status "Building RealityCore project..."
    local start_time=$(get_timestamp)
    
    cd build
    
    # Build the project
    local cpu_count=$(get_cpu_count)
    print_verbose "Using $cpu_count parallel jobs"
    
    if [ -n "$BUILD_TARGET" ]; then
        print_status "Building specific target: $BUILD_TARGET"
        if [[ "$OS" == "windows" ]] && command -v cl &> /dev/null; then
            cmake --build . --config $BUILD_CONFIG --target $BUILD_TARGET -- -maxcpucount:$cpu_count
        else
            make -j$cpu_count $BUILD_TARGET
        fi
    else
        print_status "Building all targets with progress monitoring..."
        
        # Start overall build progress monitoring
        show_build_progress "RealityCore" &
        local project_progress_pid=$!
        
        # Perform the actual build
        local build_success=false
        if [[ "$OS" == "windows" ]] && command -v cl &> /dev/null; then
            if cmake --build . --config $BUILD_CONFIG -- -maxcpucount:$cpu_count; then
                build_success=true
            fi
        else
            if make -j$cpu_count; then
                build_success=true
            fi
        fi
        
        # Stop project progress monitoring
        kill $project_progress_pid 2>/dev/null || true
        wait $project_progress_pid 2>/dev/null || true
        
        if [ "$build_success" = false ]; then
            print_error "Failed to build project"
            cd ..
            exit 1
        fi
    fi
    
    cd ..
    
    local end_time=$(get_timestamp)
    local duration=$(calculate_duration $start_time $end_time)
    print_timing "Project built in: $duration"
    print_success "RealityCore project built successfully!"
}

# Build tests only (and optionally run them)
build_tests() {
    print_status "Building unit tests..."
    local start_time=$(get_timestamp)

    # Ensure tests are configured/enabled
    print_status "Configuring CMake for tests..."
    local cmake_build_type="$BUILD_CONFIG"
    cmake -S . -B build -DENGINE_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=$cmake_build_type

    local cpu_count=$(get_cpu_count)
    print_verbose "Using $cpu_count parallel jobs"

    if [[ "$OS" == "windows" ]] && command -v cl &> /dev/null; then
        cmake --build build --config $BUILD_CONFIG --target engine_core_tests -- -maxcpucount:$cpu_count
    else
        cmake --build build --target engine_core_tests -j$cpu_count
    fi

    if [ "$RUN_TESTS" = true ]; then
        print_status "Running unit tests..."
        # Run GoogleTest binary directly in brief mode to avoid noisy CTest per-test lines
        local test_exe
        if [[ "$OS" == "windows" ]]; then
            test_exe="build/bin/engine_core_tests.exe"
        else
            test_exe="build/bin/engine_core_tests"
        fi
        if [ ! -x "$test_exe" ]; then
            print_error "Test executable not found: $test_exe"
            exit 1
        fi
        # Run without ANSI colors to simplify filtering, then hide RUN/header lines
        {
            "$test_exe" --gtest_color=no 2>&1 \
            | grep -Ev '^\[ RUN|^\[----------|^\[==========|^Running main\(|Global test environment'
        }
        local test_rc=${PIPESTATUS[0]}
        if [ $test_rc -ne 0 ]; then
            print_error "Some tests failed"
            exit 1
        fi
    fi

    local end_time=$(get_timestamp)
    local duration=$(calculate_duration $start_time $end_time)
    print_timing "Tests built in: $duration"
    print_success "Unit tests built successfully!"
}

# Setup build environment
setup_build_environment() {
    # Create build directory if it doesn't exist
    if [ ! -d "build" ]; then
        print_status "Creating build directory..."
        mkdir -p build
    fi
    
    # Ensure CMake is configured
    if [ ! -f "build/CMakeCache.txt" ]; then
        print_status "Configuring build environment..."
        cd build
        
        if [[ "$OS" == "windows" ]]; then
            if command -v cl &> /dev/null; then
                cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_POLICY_VERSION_MINIMUM=3.5
            else
                cmake .. -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5
            fi
        else
            cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        fi
        
        cd ..
    fi
}

# Main build logic
execute_build() {
    local overall_start_time=$(get_timestamp)

    # Fast path for tests-only mode
    if [ "$TESTS_ONLY" = true ]; then
        print_status "Tests-only mode (--tests-only)"
        setup_build_environment
        if ! check_dependencies; then
            build_dependencies
        fi
        build_tests
        save_build_info
        local overall_end=$(get_timestamp)
        local total=$(calculate_duration $overall_start_time $overall_end)
        print_timing "Total tests build time: $total"
        return 0
    fi
    
    case $BUILD_MODE in
        "hard")
            print_status "Full rebuild mode (--hard)"
            clean_all
            setup_build_environment
            build_dependencies
            build_project
            ;;
        "soft")
            print_status "Incremental build mode (--soft)"
            setup_build_environment
            if ! check_dependencies; then
                build_dependencies
            fi
            build_project
            ;;
        "deps-only")
            print_status "Dependencies only mode (--deps-only)"
            clean_dependencies
            setup_build_environment
            build_dependencies
            ;;
        "project-only")
            print_status "Project only mode (--project-only)"
            setup_build_environment
            if ! check_dependencies; then
                print_error "External dependencies not found! Run with --deps-only first or use --soft/--hard"
                exit 1
            fi
            clean_project
            build_project
            ;;
        "clean-deps")
            print_status "Cleaning dependencies mode (--clean-deps)"
            clean_dependencies
            return 0
            ;;
        "clean-project")
            print_status "Cleaning project mode (--clean-project)"
            clean_project
            return 0
            ;;
        "auto")
            print_status "Auto-detect build mode (smart)"
            setup_build_environment
            if ! check_dependencies; then
                print_status "Dependencies missing, building them first..."
                build_dependencies
            fi
            build_project
            ;;
        *)
            print_error "Unknown build mode: $BUILD_MODE"
            exit 1
            ;;
    esac
    
    # Save build info
    save_build_info
    
    local overall_end_time=$(get_timestamp)
    local total_duration=$(calculate_duration $overall_start_time $overall_end_time)
    print_timing "Total build time: $total_duration"
}

# Find executable and provide run instructions
find_executable() {
    print_status "Locating executables..."
    
    local demos_found=()
    
    # Look for demo executables in organized bin/ directory
    if [[ "$OS" == "windows" ]]; then
        # Windows executables with .exe extension
        for demo in build/bin/*.exe; do
            if [ -f "$demo" ]; then
                demos_found+=("$demo")
            fi
        done
    else
        # Unix executables (macOS/Linux)
        for demo in build/bin/*; do
            if [ -f "$demo" ] && [ -x "$demo" ]; then
                demos_found+=("$demo")
            fi
        done
    fi
    
    if [ ${#demos_found[@]} -gt 0 ]; then
        print_success "Build complete! Found ${#demos_found[@]} demo(s):"
        echo ""
        print_status "Available demos:"
        for demo in "${demos_found[@]}"; do
            demo_name=$(basename "$demo" .exe)  # Remove .exe extension if present
            echo "  $demo_name"
        done
        echo ""
        print_status "To run a demo:"
        if [[ "$OS" == "windows" ]]; then
            echo "  ${demos_found[0]}"
        else
            echo "  ./${demos_found[0]}"
        fi
        echo ""
        print_status "Demo information:"
        echo "  See demos/README.md for detailed descriptions of all available demos"
        echo "  Each demo showcases different aspects of the RealityCore physics engine"
    else
        if [[ "$BUILD_MODE" != "deps-only" && "$BUILD_MODE" != "clean-deps" && "$BUILD_MODE" != "clean-project" ]]; then
            print_error "No demo executables found! Build may have failed."
            exit 1
        fi
    fi
}

# Main execution
main() {
    echo "=========================================="
    echo "Enhanced RealityCore Physics Engine Build"
    echo "=========================================="
    echo ""
    
    # Parse arguments first
    parse_arguments "$@"
    
    # Show help if requested
    if [ "$SHOW_HELP" = true ]; then
        show_help
        exit 0
    fi
    
    # Show build configuration
    print_status "Build Configuration:"
    print_status "  Mode: $BUILD_MODE"
    print_status "  Config: $BUILD_CONFIG"
    if [ -n "$BUILD_TARGET" ]; then
        print_status "  Target: $BUILD_TARGET"
    fi
    if [ "$SHOW_TIMING" = true ]; then
        print_status "  Timing: enabled"
    fi
    echo ""
    
    detect_os
    check_prerequisites
    configure_git_for_large_repos
    execute_build

    if [ "$TESTS_ONLY" = true ]; then
        # In tests-only mode, skip demo discovery
        if [ "$RUN_TESTS" = false ]; then
            print_status "You can run tests with: ctest --test-dir build -j --output-on-failure"
            print_status "Or run the binary directly: build/bin/engine_core_tests"
        fi
        print_success "Tests flow completed successfully!"
    else
        find_executable
        print_success "Build process completed successfully!"
    fi
}

# Run main function
main "$@"