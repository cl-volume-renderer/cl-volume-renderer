#clw_include_once "utility_filter.cl"

__kernel void bilateral_filter(__read_only image3d_t reference_volume, __write_only image3d_t buffer){
  int4 location = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
  int4 reference_size = get_image_dim(reference_volume);
  if (location.x >= reference_size.x || location.y >= reference_size.y || location.z >= reference_size.z)
    return;
  write_imagei(buffer, location, bilateral_kernel(reference_volume, location));
}
