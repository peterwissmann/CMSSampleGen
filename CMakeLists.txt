
# which cmake to use at least, otherwise fail
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# set up cmake project
project(simProject)

# tell the compiler where to find the actual files
include_directories(${PROJECT_SOURCE_DIR}/include)

# collect names of the user code source files
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)

# tell the compiler which source files need to be compiled
add_executable(sim main.cc ${sources})

# locate Geant4 installation
find_package(Geant4 REQUIRED ui_all vis_all)

# include compilation definitions for Geant4
include(${Geant4_USE_FILE})

# link against Geant4 libraries
target_link_libraries(sim ${Geant4_LIBRARIES})
