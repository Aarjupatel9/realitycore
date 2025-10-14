# BallCollision2 CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Create the BallCollision2 executable
add_executable(BallCollision2
    main.cpp
    BallCollision2Scene.cpp
    BallCollision2CameraController.cpp
)

# Link against the RealityCore library
target_link_libraries(BallCollision2 RealityCore)

# Set include directories
target_include_directories(BallCollision2 PRIVATE
    ${CMAKE_SOURCE_DIR}/engine/include
    ${CMAKE_SOURCE_DIR}/engine/src
)

# Set C++ standard and RPATH
set_target_properties(BallCollision2 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Set RPATH to find library in ../lib/
if(APPLE)
    set_target_properties(BallCollision2 PROPERTIES
        INSTALL_RPATH "@executable_path/../lib"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
elseif(UNIX)
    set_target_properties(BallCollision2 PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()
