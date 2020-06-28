#clw_include_once "utility.cl"

__kernel void buffer_reset(__read_only image3d_t reference_volume,  __global unsigned short* buffer_volume)
{
   int3 reference_size = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};
   int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
   uint4 reset = {0, 0, 0, 0};

   if (pos.x >= reference_size.x || pos.y >= reference_size.y || pos.z >= reference_size.z)
     return;

   buffer_volume_write4(reference_size, buffer_volume, pos, reset);
}
