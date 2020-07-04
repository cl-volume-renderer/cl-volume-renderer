#clw_include_once "utility_sampling.cl"
#clw_include_once "utility_filter.cl"

__kernel void apply_clip(__read_only image3d_t original, __write_only image3d_t clipped, __global unsigned int *start, __global unsigned int *end)
{
  int4 clipped_pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
  int4 clipped_size = {end[0], end[1], end[2], 0};
  int4 pos = {start[0] + get_global_id(0), start[1] + get_global_id(1), start[2] + get_global_id(2), 0};

  if (clipped_pos.x >= clipped_size.x || clipped_pos.y >= clipped_size.y || clipped_pos.z >= clipped_size.z)
    return;

  int4 value = read_imagei(original, pos);
  write_imagei(clipped, clipped_pos, value);
}
