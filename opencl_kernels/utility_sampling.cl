float3 get_random_direction(float x, float y, float other){
  const float seed = x + y + other;
  const float3 ret = {cos(seed), sin(seed), cos(other - x - y)};
  return normalize(ret);
}

//Return a sample of a point on the hemisphere towards normal
float3 get_hemisphere_direction(float3 normal, int seed){
  float random = (get_global_id(0)+1)*(get_global_id(1)+1)*seed;
  const float3 direction = {cos(random*197), sin(random*41), cos(-random*33)};
  const float decider = dot(direction, normal);
  return normalize(direction * decider);
}

float3 get_hemisphere_direction_reflective(float3 normal, int seed, float roughness){
  float random = (get_global_id(0)+1)*(get_global_id(1)+1)*seed;
  const float3 direction = {cos(random*197), sin(random*41), cos(-random*33)};
  const float decider = dot(direction, normal);
  const float3 correct_direction = direction * decider;
  return normalize(normal*(1.0f - roughness) + direction*(roughness));
  return normalize(direction * decider);
}
