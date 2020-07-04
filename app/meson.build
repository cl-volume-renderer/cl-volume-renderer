
zlib = dependency('zlib')
gl = dependency('gl')

executable('volume-renderer',
  ['main.cpp', 'nrrd_loader.cpp',
   'volume_block.cpp', 'ui.cpp',
   'renderer.cpp', 'tf_part.h',
   'tf_part.cpp', 'hdre_loader.cpp',
   'image.cpp', 'signed_distance_field.cpp', 'reference_volume.cpp'],
  dependencies: [opencl_wrapper, stb, sdl, zlib, imgui, gl],
)

executable('sdf_benchmark',
   ['sdf_benchmark.cpp', 'nrrd_loader.cpp',
   'volume_block.cpp', 'signed_distance_field.cpp', 'reference_volume.cpp'],
  dependencies: [opencl_wrapper, zlib],
)
