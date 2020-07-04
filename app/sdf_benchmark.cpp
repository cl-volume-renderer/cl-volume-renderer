#include <signed_distance_field.h>
#include <nrrd_loader.h>
#include "debug_helper.h"

int main(int argc, char const *argv[])
{
  clw_context ctx;

  nrrd_loader loader;

  volume_block b = loader.load_file(argv[1]);

  reference_volume rv(ctx, &b);

  for (int i = 0; i < 100; ++i)
    {
       TIME_START();
       signed_distance_field(ctx, rv, "inline bool is_event_gen(short value, short gradient){ return (value > 800); }");
       TIME_PRINT("Loop Iteration");
    }

  return 0;
}
