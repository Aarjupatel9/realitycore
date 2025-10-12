# Root CMakeLists.txt - RealityCore Physics Engine
cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10...3.27)

# Set the project name
project(RealityCore VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cross-platform compiler settings
if(MSVC)
    # Visual Studio specific settings
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    # Suppress OpenGL deprecation warnings on Windows
    add_compile_definitions(GL_SILENCE_DEPRECATION)
    # Define M_PI for MinGW compatibility
    add_compile_definitions(_USE_MATH_DEFINES)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC/Clang specific settings
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(APPLE)
        # Suppress OpenGL deprecation warnings on macOS
        add_compile_definitions(GL_SILENCE_DEPRECATION)
    endif()
endif()

# Set organized output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Organize FetchContent dependencies
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/deps)

# Ensure consistent output across configurations (Windows)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach()

# Build options
option(BUILD_ENGINE_ONLY "Build only the engine library" OFF)
option(BUILD_DEMOS_ONLY "Build only the demos" OFF)
option(BUILD_LAUNCHER "Build the launcher" OFF)

# Default: build everything
if(NOT BUILD_ENGINE_ONLY AND NOT BUILD_DEMOS_ONLY)
    set(BUILD_EVERYTHING ON)
endif()

# Build the engine library
if(BUILD_EVERYTHING OR BUILD_ENGINE_ONLY)
    add_subdirectory(engine)
endif()

# Build the demos
if(BUILD_EVERYTHING OR BUILD_DEMOS_ONLY)
    add_subdirectory(demos)
endif()

# Build the launcher (future)
if(BUILD_LAUNCHER)
    add_subdirectory(launcher)
endif()

# Coverage instrumentation (GCC/Clang only)
option(ENGINE_ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)
if(ENGINE_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-O0 -g --coverage)
        add_link_options(--coverage)
    endif()
endif()

# Tests
option(ENGINE_BUILD_TESTS "Build unit tests" ON)
if(ENGINE_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print build configuration
message(STATUS "=== RealityCore Physics Engine Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
if(BUILD_EVERYTHING OR BUILD_ENGINE_ONLY)
    message(STATUS "Build engine: ON")
else()
    message(STATUS "Build engine: OFF")
endif()
if(BUILD_EVERYTHING OR BUILD_DEMOS_ONLY)
    message(STATUS "Build demos: ON")
else()
    message(STATUS "Build demos: OFF")
endif()
if(BUILD_LAUNCHER)
    message(STATUS "Build launcher: ON")
else()
    message(STATUS "Build launcher: OFF")
endif()
message(STATUS "===============================================")