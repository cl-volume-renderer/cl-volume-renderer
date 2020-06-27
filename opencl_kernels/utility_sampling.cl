#define RND_ACCURACY 2048
#define RND_ACCURACY_SUB RND_ACCURACY/2

float3 get_random_direction(float x, float y, float other){
  const float seed = x + y + other;
  const float3 ret = {cos(seed), sin(seed), cos(other - x - y)};
  return normalize(ret);
}

//A modified hashing function inspired by:
//  http://www.burtleburtle.net/bob/hash/integer.html
uint hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed <<= 3;
    seed ^= (seed >> 4);
    seed *= 0xDEADBEEF;// :D
    seed ^= (seed >> 15);
    return seed;
}


//Return a sample of a point on the hemisphere towards normal
float3 get_hemisphere_direction(float3 normal, int seed){
  unsigned int useed = (unsigned int) seed + (get_global_id(0)+1)*(get_global_id(1)+1);
  int random_access_x = hash(useed * 0x182205bd);
  int random_access_y = hash(useed * 0xe8d052f3);
  int random_access_z = hash(useed * 0xf1981dcf);
  const float3 direction = {(random_access_x % RND_ACCURACY) - RND_ACCURACY_SUB, (random_access_y % RND_ACCURACY) - RND_ACCURACY_SUB, (random_access_z % RND_ACCURACY) - RND_ACCURACY_SUB};
  const float decider = dot(direction, normal);
  const float3 correct_direction = direction * decider;

  return normalize(correct_direction);
}

float3 get_hemisphere_direction_reflective(float3 normal, int seed, float roughness){
  unsigned int useed = (unsigned int) seed + (get_global_id(0)+1)*(get_global_id(1)+1);
  int random_access_x = hash(useed * 0x182205bd);
  int random_access_y = hash(useed * 0xe8d052f3);
  int random_access_z = hash(useed * 0xf1981dcf);
  const float3 direction = {(random_access_x % RND_ACCURACY) - RND_ACCURACY_SUB, (random_access_y % RND_ACCURACY) - RND_ACCURACY_SUB, (random_access_z % RND_ACCURACY) - RND_ACCURACY_SUB};
  const float decider = dot(direction, normal);
  const float3 correct_direction = normalize(direction * decider);

  return normalize(normal*(1.0f - roughness) + correct_direction*(roughness));
}
