#clw_include_once "utility.cl"
#clw_include_once "utility_ray.cl"


__kernel void create_base_image(__read_only image3d_t reference_volume, __write_only image3d_t sdf_image){
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

  if (homogenous_neightbourhood) {
    resulting_value *= 120;
  }
  int4 result = {resulting_value, 0, 0, 0};
  write_imagei(sdf_image, location, result);
}


__kernel void create_signed_distance_field(__read_only image3d_t sdf_image, __write_only image3d_t signed_distance_field){
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
          neightbour_distance = min(neightbour_distance, (short)(tmp));
        }
      }
    }
  }

  //we only change the value when we are at a homogenous spot
  if (abs(local_value.x) > neightbour_distance + 1 && abs(added_distance) == abs_added_distance){
    local_value.x = (neightbour_distance + 1) * mul;
  }
  write_imagei(signed_distance_field, pos, local_value); //FIXME for now with unconditional write, moving this into the if causes weird bugs
}
