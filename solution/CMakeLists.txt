cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER "/usr/bin/clang-12")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-12")

project(game-of-life)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

message(STATUS "Compiler path: ${CMAKE_C_COMPILER}")
message(STATUS "Compiler ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "Compiler version: ${CMAKE_C_COMPILER_VERSION}")

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/cmake_deps)

FetchContent_Declare(
  csfml
  GIT_REPOSITORY https://github.com/SFML/CSFML.git
  GIT_TAG 2.5.2
)
FetchContent_MakeAvailable(csfml)

set(SOURCE life.c)

add_executable(application ${SOURCE})
target_link_libraries(application PRIVATE csfml-audio csfml-graphics csfml-system csfml-window)

add_executable(applicationir ${SOURCE})
target_link_libraries(applicationir PRIVATE csfml-audio csfml-graphics csfml-system csfml-window)
target_compile_options(applicationir PUBLIC -emit-llvm)

