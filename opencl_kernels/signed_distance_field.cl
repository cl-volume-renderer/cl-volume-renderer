#clw_include_once "utility.cl"
#clw_include_once "utility_ray.cl"


__kernel void create_base_image(__read_only image3d_t reference_volume, __write_only image3d_t sdf_image_ping, __write_only image3d_t sdf_image_pong, uint max_iterations){
  const sampler_t smp = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
  int4 location = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
  int4 reference_size = get_image_dim(reference_volume);

  int4 value = read_imagei(reference_volume, smp, location);
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
          int4 neightbour_value = read_imagei(reference_volume, smp, location + offset);
          if (is_event(neightbour_value.x) != is_event_result) {
            homogenous_neightbourhood = false;
          }
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

__kernel void create_signed_distance_field(__read_only image3d_t sdf_image, __write_only image3d_t signed_distance_field, int iteration, __global int *add_buffer){
  const sampler_t smp = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
  int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
  int4 reference_size = get_image_dim(sdf_image);

   if (pos.x >= reference_size.x || pos.y >= reference_size.y || pos.z >= reference_size.z)
     return;

  int4 local_value = read_imagei(sdf_image, smp, pos);
  short mul = local_value.x < 0 ? -1 : 1;
  short neightbour_distance = SHRT_MAX;
  int abs_added_distance = 0;
  int added_distance = 0;

  for(int x = -1; x < 2; x++) {
    for(int y = -1; y < 2; y++) {
      for(int z = -1; z < 2; z++) {
        if (x != 0 && y != 0 && z != 0) {
          int4 offset = {x, y, z, 0};
          int4 value = read_imagei(sdf_image, smp, pos + offset);
          short tmp = (short) abs(value.x);
          abs_added_distance += tmp;
          added_distance += value.x;
          /* this here is needed because we are walking exactly 1 pixel outside our reference volume at the max and min of each axis here
           * Based on the knowledge how step 1 did init the sdf, we know that the value 0 cannot be the case.
           * This one branche here was benchmarked against a branch for checking if we are inside the volume, the current solution was faster */
          if (tmp > 0)
            neightbour_distance = min(neightbour_distance, (short)(tmp));
        }
      }
    }
  }

  int absolut_current_value = abs(local_value.x);

  // in case our neighbourhood does contain a value from this iteration, then we are setting our own voxel to iteration +1 and multiply with mul to keep the signedness based on our evevnt location.
  if ((abs(added_distance) == abs_added_distance && neightbour_distance == iteration)) { //
    local_value.x = (iteration + 1) * mul;
    write_imagei(signed_distance_field, pos, local_value);
    atomic_inc(add_buffer);
  } else if (absolut_current_value == iteration) { //this is needed to copy over changes from the pong in the earlier iteration, so we dont write allways
    write_imagei(signed_distance_field, pos, local_value);
  }
}
