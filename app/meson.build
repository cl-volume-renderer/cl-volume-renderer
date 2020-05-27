sdl2 = dependency('sdl2')
zlib = dependency('zlib')

executable('volume-renderer',
  ['main.cpp', 'nrrd_loader.cpp', 'volume_block.cpp'],
  dependencies: [opencl_wrapper, sdl2, zlib],
)
