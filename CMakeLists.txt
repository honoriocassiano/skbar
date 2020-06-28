cmake_minimum_required(VERSION 3.1)
project(skbar)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

file(GLOB SOURCE_FILES
        "src/*.cpp"
        "src/utils/*.cpp"
        )

# LPSolve files
# lp_solve_5.5

#ADD_DEFINITIONS(-DOM_STATIC_BUILD)

ADD_DEFINITIONS(
        -DLoadInverseLib=FALSE
        -DLoadLanguageLib=FALSE
        -DLoadableBlasLib=0
        -DINVERSE_ACTIVE=INVERSE_LUSOL
        -DRoleIsExternalInvEngine
        -DNOFORTIFY
        -DCHECK_SOLUTION
        -DYY_NEVER_INTERACTIVE
        -DLPSOLVEAPIFROMLIB
        -DPARSER_LP
        -DLP_MAXLINELEN=0)
INCLUDE_DIRECTORIES(
        ./src/lp_solve_5.5
        ./src/lp_solve_5.5/bfp
        ./src/lp_solve_5.5/bfp/bfp_LUSOL
        ./src/lp_solve_5.5/bfp/bfp_LUSOL/LUSOL
        ./src/lp_solve_5.5/colamd
        ./src/lp_solve_5.5/shared)
FILE(GLOB lp_solve_src ./src/lp_solve_5.5/*.h ./src/lp_solve_5.5/*.c)
SET(lp_solve_bfp_src
        ./src/lp_solve_5.5/bfp/bfp_LUSOL/lp_LUSOL.c
        ./src/lp_solve_5.5/bfp/bfp_LUSOL/LUSOL/lusol.c)
SET(lp_solve_colamd_src
        ./src/lp_solve_5.5/colamd/colamd.h
        ./src/lp_solve_5.5/colamd/colamd.c)
FILE(GLOB lp_solve_shared_src ./src/lp_solve_5.5/shared/*.h ./src/lp_solve_5.5/shared/*.c)
SOURCE_GROUP("lp_solve_5.5" FILES ${lp_solve_src})
SOURCE_GROUP("lp_solve_5.5\\bfp" FILES ${lp_solve_bfp_src})
SOURCE_GROUP("lp_solve_5.5\\colamd" FILES ${lp_solve_colamd_src})
SOURCE_GROUP("lp_solve_5.5\\shared" FILES ${lp_solve_shared_src})
ADD_LIBRARY(lp_solve
        ${lp_solve_src}
        ${lp_solve_bfp_src}
        ${lp_solve_colamd_src}
        ${lp_solve_shared_src})

#ADD_LIBRARY(lp_solve STATIC IMPORTED)
#SET_TARGET_PROPERTIES(lp_solve PROPERTIES
#        IMPORTED_LOCATION /home/cassiano/Downloads/lp_solve_5.5.2.5_dev_ux64/liblpsolve55.a)




#include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/lp_solve_5.5)
#include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/lp_solve_5.5/bfp)
#include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/lp_solve_5.5/bfp/bfp_LUSOL)
#include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/lp_solve_5.5/bfp/bfp_LUSOL/LUSOL)
#include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/lp_solve_5.5/colamnd)
#include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/lp_solve_5.5/shared)
#
#file(GLOB_RECURSE LPSOLVE_SRC
#        "src/lp_solve_5.5/"
#
#        "src/lp_solve_5.5/*.c"
#        )
#
##SET_SOURCE_FILES_PROPERTIES( ${LPSOLVE_SRC} PROPERTIES LANGUAGE C )
#
#add_library(lpsolve ${LPSOLVE_SRC})


## KT84 files
#file(GLOB_RECURSE KT84_SRC
#        "src/kt84/"
#
#        "src/kt84/*.cpp"
#        "src/kt84/*.C"
#        "src/kt84/*.c"
#        "src/kt84/*.cc"
#        )
#
#add_library(kt84 ${KT84_SRC})
#
#include_directories(${CMAKE_SOURCE_DIR}/src/kt84)
#include_directories(${CMAKE_SOURCE_DIR}/src/kt84/geometry)
#include_directories(${CMAKE_SOURCE_DIR}/src/kt84/graphics)
#include_directories(${CMAKE_SOURCE_DIR}/src/kt84/math)
#include_directories(${CMAKE_SOURCE_DIR}/src/kt84/openmesh)

## PatchgGen files
file(GLOB_RECURSE PATCHGEN_SRC
        "src/patchgen/"

        "src/patchgen/*.cpp"
        "src/patchgen/*.C"
        "src/patchgen/*.c"
        "src/patchgen/*.cc"
        )

add_library(patchgen ${PATCHGEN_SRC})
include_directories(${CMAKE_SOURCE_DIR}/src/patchgen)

target_link_libraries(patchgen lp_solve)

# Add GLEW library
find_package(GLEW REQUIRED)

include_directories(${GLEW_INCLUDE_DIR})

# Executable config

set(TARGET_NAME skbar)

add_executable(${TARGET_NAME} ${SOURCE_FILES} src/example.h)

#set(EIGEN_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/vcglib/eigenlib")

# Compile ImGUI library
add_subdirectory("${CMAKE_SOURCE_DIR}/libsrc/imgui")

# Compile SOIL
add_subdirectory("${CMAKE_SOURCE_DIR}/libsrc/soil")

# Used to debug mode
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenMesh REQUIRED)
#find_package(LpSolve REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

include_directories(${OPENGL_INCLUDE_DIR})
# include_directories(${SDL2_INCLUDE_DIR})
# include_directories(${GLEW_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR})
#include_directories(${LPSOLVE_INCLUDE_DIR})

include_directories("${CMAKE_SOURCE_DIR}/libsrc")

include_directories(${OPENMESH_INCLUDE_DIRS})

target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic)

# target_link_libraries(sbq ${GLEW_LIBRARIES} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} pthread)
target_link_libraries(${TARGET_NAME} ${OPENGL_LIBRARIES} ${OPENMESH_LIBRARIES} pthread ${CMAKE_DL_LIBS} patchgen imgui soil ${GLEW_LIBRARIES})
