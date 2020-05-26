find_package(OpenMP REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

add_library(opencl_lib STATIC ${opencl_lib_SRC})
target_sources(opencl_lib
  PUBLIC
    "include/libmain.h"
  PRIVATE
    "src/libmain.cpp"
  )

set_target_properties(opencl_lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(opencl_lib PUBLIC ${OPENMP_INCLUDE_DIRS})
target_link_libraries(opencl_lib PRIVATE ${OpenMP_CXX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
