openmp = dependency('openmp')

opencl_wrapper_lib = static_library('opencl_wrapper',
                    join_paths('src', 'libmain.cpp'),
                    dependencies: openmp,
                 )

opencl_wrapper = declare_dependency(
  include_directories: include_directories('include'),
  link_with: opencl_wrapper_lib,
)
