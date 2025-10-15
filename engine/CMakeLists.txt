# RealityCore Physics Engine Shared Library CMakeLists.txt
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

# --- Library Integration using FetchContent ---
include(FetchContent)

# GLFW (Windowing and Input)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLAD (OpenGL Loader)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLM (Mathematics Library)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# Bullet Physics (Physics Engine)
FetchContent_Declare(
  bullet3
  GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
  GIT_TAG 3.25
)
# Configure Bullet Physics build options
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
set(BUILD_ENET OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET_ROBOTICS_GUI_EXTRA OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET_ROBOTICS_EXTRA OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
set(BUILD_PYBULLET_NUMPY OFF CACHE BOOL "" FORCE)
set(USE_DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(bullet3)

# Find OpenGL
find_package(OpenGL REQUIRED)

# --- Engine Source Files ---
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")

# Create shared library
add_library(RealityCore SHARED ${ENGINE_SOURCES})

# Set library properties
set_target_properties(RealityCore PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "include/RigidBody3D.h;include/Vec2.h;include/World.h"
)

# Set proper RPATH for library discovery
if(APPLE)
    set_target_properties(RealityCore PROPERTIES
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
elseif(UNIX)
    set_target_properties(RealityCore PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

# Include directories
target_include_directories(RealityCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
    $<BUILD_INTERFACE:${glad_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${bullet3_SOURCE_DIR}/src>
)

# Link libraries
target_link_libraries(RealityCore PUBLIC
    glad
    ${OPENGL_LIBRARIES}
    glfw
    BulletDynamics
    BulletCollision
    BulletSoftBody
    LinearMath
)

# Export targets for use by demos
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
