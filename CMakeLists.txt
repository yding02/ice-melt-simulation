cmake_minimum_required(VERSION 3.1)
project(IceSim LANGUAGES CXX)

#-------------------------------------------------------------------------------
# Set C++ version
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#-------------------------------------------------------------------------------
# Disable GLFW extras
#-------------------------------------------------------------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#-------------------------------------------------------------------------------
# Add includes
#-------------------------------------------------------------------------------
include_directories(include)

#-------------------------------------------------------------------------------
# Add subdirectories
#-------------------------------------------------------------------------------
add_subdirectory(ext/glad)
add_subdirectory(ext/glfw)
add_subdirectory(src)

# Install settings
set(CMAKE_INSTALL_PREFIX "${IceSim_SOURCE_DIR}/")
