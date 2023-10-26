cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER clang-12)

project(GameOfLife)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

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

add_subdirectory(src)

add_subdirectory(apps)
