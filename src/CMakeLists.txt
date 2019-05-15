cmake_minimum_required(VERSION 3.1)

#-------------------------------------------------------------------------------
# Set source files
#-------------------------------------------------------------------------------
set(ICESIM_VIEWER_SOURCE
    # Ice simulation objects
    ice.cpp
    iceVoxel.cpp

    # Data Structures
    data_struct.h
    data_struct.cpp

    # Application
    main.cpp
    iceSimulator.cpp
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable(icesim ${ICESIM_VIEWER_SOURCE})

target_link_libraries(icesim
    glad
    glfw
)

# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)

# Install to project root
install(TARGETS icesim DESTINATION ${IceSim_SOURCE_DIR})
