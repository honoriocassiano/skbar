cmake_minimum_required(VERSION 2.8)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# file(GLOB_RECURSE SOURCE_FILES
#     "*.hpp"
#     "*.h"
#     "*.cpp"
#     "*.C"
#     "*.c"
# )

set(SOURCE_FILES
	"SOIL.c"
	"image_DXT.c"
	"image_helper.c"
	"stb_image_aug.c"

)

# find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
# include_directories(${SDL2_INCLUDE_DIR})

add_library(soil SHARED ${SOURCE_FILES})
target_link_libraries(soil ${OPENGL_LIBRARIES})
# target_link_libraries(imgui ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
