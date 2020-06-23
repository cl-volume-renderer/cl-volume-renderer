
zlib = dependency('zlib')
gl = dependency('gl')

executable('volume-renderer',
  ['main.cpp', 'nrrd_loader.cpp', 'volume_block.cpp', 'ui.cpp', 'renderer.cpp', 'tf_part.h', 'tf_part.cpp', 'hdre_loader.cpp', 'image.cpp'],
  dependencies: [opencl_wrapper, stb, sdl, zlib, imgui, gl],
)
