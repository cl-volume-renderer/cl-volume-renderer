#include "signed_distance_field.h"
#include "debug_helper.h"
#include <algorithm>
#include <clw_vector.h>
#include <clw_function.h>

signed_distance_field::signed_distance_field(clw_context &c, const reference_volume &rv, std::string local_cl_code) :
  sdf(c, std::move(std::vector<char>(rv.get_volume_length())), rv.get_volume_size(), true) {
  TIME_START();
  //this assumes that *at least* there is 1 element in the middle of the longest axis.
  size_t max_iterations = std::min((*std::max_element(rv.get_volume_size().begin(), rv.get_volume_size().end()))/2, (size_t)127);
  auto sdf_pong = clw_image<char>(c, std::vector<char>(rv.get_volume_length()), rv.get_volume_size());

  //first fill the sdf image with -1 for *inside* a interesting area 1 outside a interesting area, and max iteration when the neightbourhood is homogenous
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
    //only exit when pong is the sdf buffer.
    if (atomic_add_buffer[0] == 0 && i % 2 == 1) {
      break;
    }
  }

  TIME_PRINT("sdf calc time");
}


clw_image<char>& signed_distance_field::get_sdf_buffer() {
   return sdf;
}

signed_distance_field::signed_distance_field(clw_context &c) :
  sdf(c, std::vector<char>(8, 0), {2,2,2}, true) {

}

#if 0

  if (local_pos.x == 0) {
    int4 offset = {-1, 0, 0, 0};
    local_lookup[get_local_id(0) + 1 - 1][get_local_id(1) + 1][get_local_id(2) + 1] = is_event(read_imagei(reference_volume, smp, location + offset).x);
  }
  if (local_pos.x == local_max_pos.x - 1) {
    int4 offset = {1, 0, 0, 0};
    local_lookup[get_local_id(0) + 1 + 1][get_local_id(1) + 1][get_local_id(2) + 1] = is_event(read_imagei(reference_volume, smp, location + offset).x);
  }

  if (local_pos.y == 0) {
    int4 offset = {0, -1, 0, 0};
    local_lookup[get_local_id(0) + 1][get_local_id(1) + 1 - 1][get_local_id(2) + 1] = is_event(read_imagei(reference_volume, smp, location + offset).x);
  }
  if (local_pos.y == local_max_pos.y - 1) {
    int4 offset = {0, +1, 0, 0};
    local_lookup[get_local_id(0) + 1][get_local_id(1) + 1 + 1][get_local_id(2) + 1] = is_event(read_imagei(reference_volume, smp, location + offset).x);
  }

  if (local_pos.z == 0) {
    int4 offset = {0, 0, -1, 0};
    local_lookup[get_local_id(0) + 1][get_local_id(1) + 1][get_local_id(2) + 1 - 1] = is_event(read_imagei(reference_volume, smp, location + offset).x);
  }
  if (local_pos.z == local_max_pos.z - 1) {
    int4 offset = {0, 0, +1, 0};
    local_lookup[get_local_id(0) + 1][get_local_id(1) + 1][get_local_id(2) + 1 + 1] = is_event(read_imagei(reference_volume, smp, location + offset).x);
  }

#endif
