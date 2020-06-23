#clw_include_once "utility_ray.cl"

float4 sample_environment_map(struct ray sample_ray, __read_only image2d_t environment_map){
  const sampler_t smp = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
  const float2 inverse_pi_factor = {0.318309886f, 0.1591549431f};
  const float2 offset = {0.5f, 0.5f};
  float2 uv_coordinate = {atan(sample_ray.direction.z/sample_ray.direction.x), asin(sample_ray.direction.y)};
  uv_coordinate *= inverse_pi_factor;
  uv_coordinate += offset;
  const float4 read_colour = read_imagef(environment_map, smp, uv_coordinate);
  return read_colour;
}
