cmake_minimum_required(VERSION 2.8)
project(correspondence_matching_pro)

set(CMAKE_C_FLAGS "-m64")
set(CMAKE_CXX_FLAGS "-std=c++11 -m64")
#set(LIB_DIR "/opt/local/lib")

set(CMAKE_CXX_FLAGS "-std=c++11 -fno-operator-names")

option(USE_SIMD_SSE2 "Enable SSE2 optimizations" OFF)
option(USE_SIMD_SSE3 "Enable SSE3 optimizations" OFF)
option(USE_SIMD_SSSE3 "Enable SSSE3 optimizations" OFF)
option(USE_SIMD_SSE4_1 "Enable SSE 4.1 optimizations" ON)
option(USE_SIMD_SSE4_2 "Enable SSE 4.2 optimizations" ON)
option(USE_SIMD_AVX "Enable AVX optimizations" ON)
option(USE_SIMD_AVX2 "Enable AVX2 optimizations" ON)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if (USE_SIMD_SSE4_1)
    add_definitions(-DUSE_SSE)
    message(STATUS "Use SSE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
endif()

if (USE_SIMD_AVX2)
    add_definitions(-DGLM_FORCE_PURE)
    if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        add_compile_options(-mavx2)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
        add_compile_options(/QxAVX2)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_options(/arch:AVX2)
    endif()
endif()

#set(CUDA_TOOLKIT_ROOT_DIR "/usr/local/cuda")
#set(CUDA_INCLUDE_DIR "/usr/local/cuda/include")
#set(CUDA_LIBRARY "/usr/local/cuda/lib64")

set(OpenCV_DIR "/Users/willard/libs/opencv/4.6.0/lib/cmake/opencv4")
option( OpenCV_STATIC OFF )
find_package(OpenCV REQUIRED)

link_directories( ${OpenCV_LIB_DIR} )
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

#set(Faiss_INCLUDE_DIRS "/raid/yuanyong/libs/faiss-1.5.3-build/include")
#set(Faiss_LIBS_DIRS "/raid/yuanyong/libs/faiss-1.5.3-build/lib/libfaiss.so")
#include_directories(${Faiss_INCLUDE_DIRS})

file(GLOB correspondence_matching_files
    "./include/svf.h"
    "./src/svf.cc"
    "./examples/opencv_sift_matching.cc"
)

add_executable(correspondence_matching_demo ${correspondence_matching_files})
target_link_libraries(correspondence_matching_demo ${OpenCV_LIBS})
