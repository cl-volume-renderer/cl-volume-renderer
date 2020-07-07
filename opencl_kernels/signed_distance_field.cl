#clw_include_once "utility.cl"
#clw_include_once "utility_ray.cl"


__kernel void create_base_image(__read_only image3d_t reference_volume, __write_only image3d_t sdf_image_ping, __write_only image3d_t sdf_image_pong, uint max_iterations){
  int4 location = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
  int4 reference_size = get_image_dim(reference_volume);
  int4 tmp_for_minus_ones = {-1, -1, -1, -1};
  int4 max_coords = reference_size + tmp_for_minus_ones;
  int4 min_coords = {0, 0, 0, 0};

  int4 value = read_imagei(reference_volume, location);
  int resulting_value = 0;

  if (location.x >= reference_size.x || location.y >= reference_size.y || location.z >= reference_size.z)
   return;
  bool is_event_result = is_event(value.x);
  if (is_event_result) {
    resulting_value = -1;
  }else {
    resulting_value = 1;
  }
  bool homogenous_neightbourhood = true;

  //check if the surrounding voxels are all either within a event area or outside a event area
  for (int x = -1; x < 2; x++) {
    for (int y = -1; y < 2; y++) {
      for (int z = -1; z < 2; z++) {
        if (x != 0 && y != 0 && z != 0) {
          int4 offset = {x, y, z, 0};
          int4 relative_pos = clamp(offset + location, min_coords, max_coords);
          int4 neightbour_value = read_imagei(reference_volume, relative_pos);
          bool neightbour_event = is_event(neightbour_value.x);
          homogenous_neightbourhood = homogenous_neightbourhood & (neightbour_event == is_event_result);
        }
      }
    }
  }

  //If everything is homogenous we will write our max iteration value into it. but the borders between different areas will stay with 1 or -1
  if (homogenous_neightbourhood) {
    resulting_value *= max_iterations;
  }

  //finally write to ping as well as pong
  int4 result = {resulting_value, 0, 0, 0};
  write_imagei(sdf_image_ping, location, result);
  write_imagei(sdf_image_pong, location, result);
}

inline short
neightbour_distance_calc(__read_only image3d_t sdf_image, int4 pos) {
  char neightbour_distance = CHAR_MAX;
  int abs_added_distance = 0;
  int added_distance = 0;
  int4 reference_size = get_image_dim(sdf_image);
  int4 tmp_for_minus_ones = {-1, -1, -1, -1};
  int4 max_coords = reference_size + tmp_for_minus_ones;
  int4 min_coords = {0, 0, 0, 0};

  for(int x = -1; x < 2; x++) {
    for(int y = -1; y < 2; y++) {
      for(int z = -1; z < 2; z++) {
        if (x != 0 && y != 0 && z != 0) {
          int4 offset = {x, y, z, 0};
          int4 relative_pos = clamp(offset + pos, min_coords, max_coords);
          int4 value = read_imagei(sdf_image, relative_pos);
          char tmp = (char) abs(value.x);
          abs_added_distance += tmp;
          added_distance += value.x;
          neightbour_distance = min(neightbour_distance, (char)(tmp));
        }
      }
    }
  }
  if (abs(added_distance) == abs_added_distance)
    return neightbour_distance;
  else
    return 0;
}

__kernel void create_signed_distance_field(__read_only image3d_t sdf_image, __write_only image3d_t signed_distance_field, int iteration, __global int *add_buffer, int max_iterations){
  int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
  int4 reference_size = get_image_dim(sdf_image);

  int4 local_value = read_imagei(sdf_image, pos);
  int absolut_current_value = abs(local_value.x);

  if (pos.x >= reference_size.x || pos.y >= reference_size.y || pos.z >= reference_size.z || absolut_current_value < iteration)
   return;

  if (absolut_current_value == iteration) {
    write_imagei(signed_distance_field, pos, local_value);
    return;
  }

  char mul = local_value.x < 0 ? -1 : 1;
  char neightbour_distance = neightbour_distance_calc(sdf_image, pos);

  // in case our neighbourhood does contain a value from this iteration, then we are setting our own voxel to iteration +1 and multiply with mul to keep the signedness based on our evevnt location.
  if ((neightbour_distance != 0 && neightbour_distance == iteration)) { //
    local_value.x = (iteration + 1) * mul;
    write_imagei(signed_distance_field, pos, local_value);
    atomic_inc(add_buffer);
  }
}
