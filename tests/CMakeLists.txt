cmake_minimum_required(VERSION 3.10)

# GoogleTest via FetchContent (pinned)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# Always build googletest without installing
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing (handled in root but safe here)
include(GoogleTest)

# Common test sources
add_executable(engine_core_tests
    core/test_rigidbody_integration.cpp
    core/test_world_update.cpp
    core/test_inertia_tensor.cpp
    core/test_collision_system.cpp
    core/test_object_pool.cpp
    core/test_world_multistep.cpp
)

# Allow including engine internal headers for now
# TODO: move to public API when headers are consolidated
target_include_directories(engine_core_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/engine/src
)

# C++ standard consistent with project
set_target_properties(engine_core_tests PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

# Include directories: rely on RealityCore's PUBLIC headers
# RealityCore target provides include dirs via target_link_libraries

target_link_libraries(engine_core_tests PRIVATE
    gtest_main
    RealityCore
)

gtest_discover_tests(engine_core_tests DISCOVERY_TIMEOUT 60)
