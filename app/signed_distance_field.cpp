#include "signed_distance_field.h"

signed_distance_field::signed_distance_field(clw_context &c, const reference_volume &rv, std::string local_cl_code) :
  sdf(c, std::move(std::vector<short>(rv.get_volume_length())), rv.get_volume_size(), true) {
  auto sdf_pong = clw_image<short>(c, std::vector<short>(rv.get_volume_length()), rv.get_volume_size());

  //first fill the sdf image with -1 for *inside* a interesting area 1 outside a interesting area
  auto sdf_generation_init = clw_function(c, "signed_distance_field.cl", "create_boolean_image", local_cl_code);
  sdf_generation_init.execute(rv.get_volume_size_evenness(8), {4,4,4}, rv.get_reference_volume(), sdf);

  //now iterate

  clw_vector<int> atomic_add_buffer(c, std::vector<int>(1,0));
  auto sdf_generation_initialization = clw_function(c, "signed_distance_field.cl", "create_signed_distance_field");
  printf("Starting to build SDF\n");
  clock_t start = clock();
  for (int i = 0; i < 1000; ++i) {
    atomic_add_buffer[0] = 0;
    atomic_add_buffer.push();
    //FIXME this is only needed because swap does not like clw_image
    if (i % 2 == 0)
      sdf_generation_initialization.execute(rv.get_volume_size_evenness(8), {4,4,4}, sdf, sdf_pong, atomic_add_buffer);
    else
      sdf_generation_initialization.execute(rv.get_volume_size_evenness(8), {4,4,4}, sdf_pong, sdf, atomic_add_buffer);
    atomic_add_buffer.pull();
    if (atomic_add_buffer[0] == 0 && i % 2 == 1) { //FIXME the later part is only required because sdf is not always filled with the correct values
      break;
    }
  }
  printf("Finished building SDF (%f)\n", (float)(clock() - start) / CLOCKS_PER_SEC);

  //this here is needed for nvidia to ensure that in another call with sdf as parameter to not contain gargabe
  sdf.pull();
  sdf.push();
}

clw_image<short>& signed_distance_field::get_sdf_buffer() {
   return sdf;
}

signed_distance_field::signed_distance_field(clw_context &c) :
  sdf(c, std::vector<short>(8, 0), {2,2,2}, true) {

}
