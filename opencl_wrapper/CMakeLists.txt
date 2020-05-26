find_package(OpenMP REQUIRED)
find_package(SDL2 REQUIRED)

add_library(opencl_wrapper STATIC)
target_sources(opencl_wrapper
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/libmain.cpp"
  )

set_target_properties(opencl_wrapper PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(opencl_wrapper PRIVATE ${OPENMP_INCLUDE_DIRS} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(opencl_wrapper PRIVATE ${OpenMP_CXX_LIBRARIES})
