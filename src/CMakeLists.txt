cmake_minimum_required(VERSION 2.8)

# Ice simulation source
set(ICESIM_VIEWER_SOURCE
    # Ice simulation objects
    ice.cpp
    iceVoxel.cpp

    # Application
    main.cpp
    iceSimulator.cpp

    # Camera
    # camera.cpp
)

# Windows-only sources
if(WIN32)
list(APPEND ICESIM_VIEWER_SOURCE

)
endif(WIN32)

#-------------------------------------------------------------------------------
# Embed resources
#-------------------------------------------------------------------------------

# embed_resources(ICESIM_SHADERS
#   shaders/vert.glsl
#   shaders/normalFrag.glsl
#   shaders/wireframeFrag.glsl
# )

#-------------------------------------------------------------------------------
# Set definitions
#-------------------------------------------------------------------------------
add_definitions(${NANOGUI_EXTRA_DEFS})

#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
include_directories(
  ${CGL_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
  #${NANOGUI_EXTRA_INCS}
)

#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
  ${CGL_LIBRARY_DIRS}
  ${FREETYPE_LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable(icesim ${ICESIM_VIEWER_SOURCE})

target_link_libraries(icesim
    CGL ${CGL_LIBRARIES}
    #nanogui ${NANOGUI_EXTRA_LIBS}
    ${FREETYPE_LIBRARIES}
    ${CMAKE_THREADS_INIT}
)

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if(APPLE)
  set_property( TARGET icesim APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)

# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)

# Install to project root
install(TARGETS icesim DESTINATION ${IceSim_SOURCE_DIR})