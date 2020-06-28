project('gpgpu-volume-renderer',
  'cpp',
  default_options: ['cpp_std=c++17'],
  version: '0.1'
)

add_global_arguments('-DKERNEL_DIR="' + join_paths(meson.current_source_dir(), 'opencl_kernels/')+'"', language : 'cpp')

if get_option('buildtype') != 'release'
  add_project_arguments('-Og', language: 'cpp')
endif


sdl = dependency('sdl2')

imgui_proj = subproject('imgui')
imgui = imgui_proj.get_variable('imgui_dep')

stb_proj = subproject('stb')
stb = stb_proj.get_variable('stb_dep')

subdir('opencl_wrapper')
subdir('app')

