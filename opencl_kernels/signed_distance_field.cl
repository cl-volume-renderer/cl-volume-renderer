
__kernel void create_boolean_image(__read_only image3d_t reference_volume, __write_only image3d_t sdf_image){
   const sampler_t smp = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
   int4 location = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
   int4 reference_size = get_image_dim(reference_volume);

   int4 value = read_imagei(reference_volume, smp, location);
   int4 result = {0, 0, 0, 0};

   if (location.x >= reference_size.x || location.y >= reference_size.y || location.z >= reference_size.z)
     return;

   //FIXME this is not really great right now how it is.
   //FIXME we should somehow abstract out this value.x > X thing
   if (value.x > 800) {
     result.x = -1;
   }else {
     result.x = 1;
   }
   write_imagei(sdf_image, location, result);
}

__kernel void create_signed_distance_field(__read_only image3d_t sdf_image, __write_only image3d_t signed_distance_field, __global int *change_counter){
  const sampler_t smp = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
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
          neightbour_distance = min(neightbour_distance, (short)(tmp + 1));
        }
      }
    }
  }

  neightbour_distance = neightbour_distance * mul;
  //we only change the value when we are at a homogenous spot
  if (local_value.x != neightbour_distance && abs(added_distance) == abs_added_distance){
    local_value.x = neightbour_distance;
    atomic_add(change_counter, 1);
  }
  write_imagei(signed_distance_field, pos, local_value);
}
