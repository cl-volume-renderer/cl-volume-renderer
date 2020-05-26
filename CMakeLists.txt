cmake_minimum_required (VERSION 3.8.11)
project (gpgpu-volume-renderer)

#Project Setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -Wall -Wextra -fsanitize=address")

file(GLOB app_SRC
  "app/*.h"
  "app/*.hpp"
  "app/*.cpp"
  )

add_executable(volume-renderer "app/main.cpp" ${app_SRC})

add_subdirectory(opencl_wrapper)
