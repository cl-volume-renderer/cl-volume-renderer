test_config = configuration_data()
test_config.set_quoted('TEST_FILE', meson.current_source_dir() / 'testdata.nrrd')

configure_file(
  output: 'test_config.hpp',
  configuration: test_config,
)

sdf_test = executable('sdf_test',
  ['sdf_test.cpp'],
  dependencies: [app_source],
)

test('sdf', sdf_test)
