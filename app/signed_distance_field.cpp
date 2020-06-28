#include "signed_distance_field.h"
#include "debug_helper.h"

signed_distance_field::signed_distance_field(clw_context &c, const reference_volume &rv, std::string local_cl_code) :
  sdf(c, std::move(std::vector<short>(rv.get_volume_length())), rv.get_volume_size(), true) {
  TIME_START();
  size_t max_iterations = std::min((*std::max_element(rv.get_volume_size().begin(), rv.get_volume_size().end()))/2, (size_t)127);
  auto sdf_pong = clw_image<short>(c, std::vector<short>(rv.get_volume_length()), rv.get_volume_size());

  //first fill the sdf image with -1 for *inside* a interesting area 1 outside a interesting area
  auto sdf_generation_init = clw_function(c, "signed_distance_field.cl", "create_base_image", local_cl_code);
  sdf_generation_init.execute(rv.get_volume_size_evenness(8), {4,4,4}, rv.get_reference_volume(), sdf, sdf_pong, (unsigned int)max_iterations);

  //now iterate
  auto sdf_generation_initialization = clw_function(c, "signed_distance_field.cl", "create_signed_distance_field", local_cl_code);
  auto *ping = &sdf, *pong = &sdf_pong;
  clw_vector<int> atomic_add_buffer(c, std::vector<int>(1,0));
  for (unsigned int i = 1; i <= max_iterations+(max_iterations % 2)+1; ++i) {
    atomic_add_buffer[0] = 0;
    atomic_add_buffer.push();
    sdf_generation_initialization.execute(rv.get_volume_size_evenness(8), {4,4,4}, *ping, *pong, i, atomic_add_buffer, (unsigned int)max_iterations);
    std::swap(ping, pong);
    atomic_add_buffer.pull();
    if (atomic_add_buffer[0] == 0 && i % 2 == 1) {
      break;
    }
  }

  TIME_PRINT("sdf calc time");
}


clw_image<short>& signed_distance_field::get_sdf_buffer() {
   return sdf;
}

signed_distance_field::signed_distance_field(clw_context &c) :
  sdf(c, std::vector<short>(8, 0), {2,2,2}, true) {

}
