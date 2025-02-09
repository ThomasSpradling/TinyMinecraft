cmake_minimum_required(VERSION 3.10)
project(MinecraftClone)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O1 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_compile_options(
  # -pedantic-errors
  # -fno-gnu-keywords
  # -Wall
  # -Wextra
)

### Configure Dependencies

set(BUILD_SHARED_LIBS ON CACHE BOOL "Make all libs dynamic" FORCE)

# GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory("third_party/glfw")

# GLM
add_subdirectory("third_party/glm")

# GLAD
add_library(GLAD_LIB "third_party/glad/src/glad.c")

# JSON
add_subdirectory("third_party/json")

# FastNoise2
set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE)
add_subdirectory("third_party/FastNoise2")

# OneTBB
set(TBBMALLOC_BUILD OFF CACHE BOOL "Malloc build" FORCE)
set(TBB_TEST OFF CACHE BOOL "Test build" FORCE)

add_subdirectory("third_party/TBB")

include_directories(
  "include"
  third_party/glad/include
  third_party/glfw/include
  third_party/glm
  third_party/json/include
  third_party/FastNoise2/include
  third_party/TBB/include
  third_party
  third_party/stb
  third_party/noise
)

if (APPLE)
  set(FRAMEWORKS "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/**/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} glfw GLAD_LIB FastNoise TBB::tbb ${FRAMEWORKS})

target_compile_options(GLAD_LIB PRIVATE -w)
target_compile_options(FastNoise PRIVATE -w)
target_compile_options(glfw PRIVATE -w)
target_compile_options(glm PRIVATE -w)
