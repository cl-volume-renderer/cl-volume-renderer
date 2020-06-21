
zlib = dependency('zlib')
gl = dependency('gl')

executable('volume-renderer',
  ['main.cpp', 'nrrd_loader.cpp', 'volume_block.cpp', 'ui.cpp', 'renderer.cpp', 'tf_part.h', 'tf_part.cpp'],
  dependencies: [opencl_wrapper, sdl, zlib, imgui, gl],
)
