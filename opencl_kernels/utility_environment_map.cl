#clw_include_once "utility_ray.cl"

uint4 sample_environment_map(struct ray sample_ray, __read_only image2d_t environment_map){
  const sampler_t smp = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_NORMALIZED_COORDS_TRUE;
  const float aspect = get_image_width(environment_map)/get_image_height(environment_map);
  const float2 inverse_pi_factor = {0.1591549431f, 0.318309886f};
  const float2 offset = {0.5f, 0.5f};
  float2 uv_coordinate = {atan2(sample_ray.direction.x,sample_ray.direction.z), asin(-sample_ray.direction.y)};
  uv_coordinate *= inverse_pi_factor;
  uv_coordinate += offset;
  const uint4 read_colour = read_imageui(environment_map, smp, uv_coordinate);
  return read_colour;
}
float4 sample_environment_map_e(struct ray sample_ray, __read_only image2d_t environment_map){
  const sampler_t smp = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_NORMALIZED_COORDS_TRUE;
  //const float aspect = get_image_width(environment_map)/get_image_height(environment_map);
  const float2 inverse_pi_factor = {0.318309886f, 0.1591549431f};
  const float2 offset = {0.5f, 0.5f};
  float2 uv_coordinate = {atan(sample_ray.direction.z/sample_ray.direction.x), asin(-sample_ray.direction.y)};
  uv_coordinate *= inverse_pi_factor;
  uv_coordinate += offset;
  const uint4 read_colour = read_imageui(environment_map, smp, uv_coordinate);

  float r = (float)read_colour.x/255.f;
  float g = (float)read_colour.y/255.f;
  float b = (float)read_colour.z/255.f;
  //watts/steradian/meter^2
  //https://www.graphics.cornell.edu/~bjw/rgbe.html
  float exp = ldexp(1.0f, read_colour.w - (128 + 8));
  r = r * exp;
  g = g * exp;
  b = b * exp;
  
  if(get_global_id(0) == 0 && get_global_id(1) == 0){
    printf("exp: %f", exp);
  }

  float4 watt_per_seradian_per_metre_sq = {r,g,b,exp};
  return watt_per_seradian_per_metre_sq;
}
