cmake_minimum_required(VERSION 3.10)
project(MinecraftClone)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(
  -Wpedantic
  -Wall
  -Wextra
  -Wcast-align
  -Wcast-qual
  -Wctor-dtor-privacy
  -Wdisabled-optimization
  -Wformat=2
  -Winit-self
  -Wmissing-declarations
  -Wmissing-include-dirs
  -Woverloaded-virtual
  -Wredundant-decls
  -Wshadow
  -Wsign-conversion
  -Wsign-promo
  -Wstrict-overflow=5
  -Wswitch-default
  -Wimplicit-fallthrough
  -Wundef
  -Werror
)

### Configure Dependencies

# GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory("third_party/glfw")

# GLM
add_subdirectory("third_party/glm")

# GLAD
add_library(GLAD_LIB "third_party/glad/src/glad.c")

include_directories(
  "include"
  third_party/glad/include
  third_party/glfw/include
  third_party/glm
  third_party
)

if (APPLE)
  set(FRAMEWORKS "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} glfw GLAD_LIB ${FRAMEWORKS})

target_compile_options(${PROJECT_NAME} PRIVATE 
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:RELEASE>:-DNDEBUG -O2>
)

target_compile_options(GLAD_LIB PRIVATE -w)
target_compile_options(glfw PRIVATE -w)
target_compile_options(glm PRIVATE -w)
