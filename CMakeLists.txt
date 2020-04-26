cmake_minimum_required(VERSION 3.1)
project(skbar)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB SOURCE_FILES
        "src/*.cpp"
        "src/*.C"
        "src/*.c"
        )

add_executable(skbar ${SOURCE_FILES})

set(EIGEN_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/vcglib/eigenlib")

# Compile ImGUI library
# add_subdirectory("${CMAKE_SOURCE_DIR}/libsrc/imgui")

# Compile SOIL
# add_subdirectory("${CMAKE_SOURCE_DIR}/libsrc/soil")

# Used to debug mode
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# find_package(GLEW REQUIRED)
# find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenMesh REQUIRED)
#find_package(LIBIGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
# include_directories(${SDL2_INCLUDE_DIR})
# include_directories(${GLEW_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OPENMESH_INCLUDE_DIRS})
#include_directories(${LIBIGL_INCLUDE_DIR})

# target_link_libraries(sbq ${GLEW_LIBRARIES} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} pthread)
target_link_libraries(skbar ${OPENGL_LIBRARIES} ${OPENMESH_LIBRARIES} pthread)
