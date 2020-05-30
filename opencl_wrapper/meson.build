openmp = dependency('openmp')
opencl = dependency('OpenCL')
opencl = dependency('gl')

opencl_wrapper_lib = static_library('opencl_wrapper',
                    [join_paths('src', 'libmain.cpp'),
                     join_paths('src', 'clw_context.cpp')],
                    include_directories: include_directories('include'),
                    dependencies: [openmp, opencl, gl],
                 )

opencl_wrapper = declare_dependency(
  include_directories: include_directories('include'),
  link_with: opencl_wrapper_lib,
)
