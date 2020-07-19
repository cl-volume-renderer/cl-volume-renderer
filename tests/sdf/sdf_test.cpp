#include <signed_distance_field.hpp>
#include <nrrd_loader.hpp>
#include <debug_helper.hpp>
#include "test_config.h"

const size_t expected_length = 50540;

const int values[] = {
  #include "values.x"
};

int main(int argc, char const *argv[])
{
  clw_context ctx;

  nrrd_loader loader;

  volume_block b = loader.load_file(TEST_FILE);

  reference_volume rv(ctx, &b);

  auto sdf = signed_distance_field(ctx, rv, "inline bool is_event_gen(short value, short gradient, uint4 *color){ return (value > 800); }");
  auto sdf_image = sdf.get_sdf_buffer();

  assert(sdf_image.size() == expected_length);
  sdf_image.pull();

  for (unsigned int i = 0; i < expected_length; ++i)
    {
       assert(values[i] == sdf_image[i]);
    }
  std::cout << "EVERYTHING FINE\n";
  return 0;
}
