project('gpgpu-volume-renderer',
  'cpp',
  default_options: ['cpp_std=c++17'],
  version: '0.1'
)

sdl = dependency('sdl2')

imgui_proj = subproject('imgui')
imgui = imgui_proj.get_variable('imgui_dep')
subdir('opencl_wrapper')
subdir('app')
