
zlib = dependency('zlib')
gl = dependency('gl')

app_source = declare_dependency(
  include_directories : include_directories('.'),
  sources: ['nrrd_loader.cpp',
   'volume_block.cpp', 'ui.cpp',
   'renderer.cpp', 'tf_part.h',
   'tf_part.cpp', 'hdre_loader.cpp',
   'image.cpp', 'signed_distance_field.cpp', 'reference_volume.cpp'],
  dependencies: [opencl_wrapper, stb, sdl, zlib, imgui, gl],
)

executable('volume-renderer',
  ['main.cpp'],
  dependencies: app_source,
)

executable('sdf_benchmark',
   ['sdf_benchmark.cpp'],
  dependencies: [app_source],
)
