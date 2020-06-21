#define MAX_STEP_COUNT 1000
#clw_include_once "utility.cl"
#clw_include_once "utility_ray.cl"
#clw_include_once "utility_sampling.cl"
#clw_include_once "utility_filter.cl"

//Expect the surface ray as entry point
uint4 compute_ao(struct ray surface_ray, __read_only image3d_t reference_volume, __read_only image3d_t sdf, __global char* buffer_volume, int random_seed){
  const sampler_t smp = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
  const int3 reference_dimensions = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};

  float hit_accumulator = 0.0;

  struct ray current_ray = surface_ray;
  struct ray hit_information = {0};
  int2 buffer_value = {0,0};

  enum event ray_event;
  current_ray = march_to_next_event(surface_ray, reference_volume, sdf, &ray_event);
  if(ray_event == Hit){
    buffer_value = buffer_volume_readf(reference_dimensions, buffer_volume, make_float4(current_ray.origin,0));
    hit_information = current_ray;
    //Compute AO further?
    if(buffer_value.x < 100){
      buffer_value.x += 1;

      current_ray = ray_bounce(current_ray, -normalize(gradient_prewitt_nn(reference_volume, make_float4(current_ray.origin,0))),random_seed);
      current_ray = march(current_ray,sdf);
      current_ray = march(current_ray,sdf);
      current_ray = march(current_ray,sdf);
      current_ray = march(current_ray,sdf);
      current_ray = march(current_ray,sdf);
      current_ray = march(current_ray,sdf);
      current_ray = march(current_ray,sdf);
      /////AO ray-shooting

      march_to_next_event(current_ray, reference_volume,sdf, &ray_event);
      if(ray_event == Hit){
        buffer_value.y += 1;
      }
      buffer_volume_writef(reference_dimensions, buffer_volume, make_float4(hit_information.origin, 0), buffer_value);
    }
  }else{
    return 0.0;
  }


  buffer_value = buffer_volume_readf(reference_dimensions, buffer_volume, make_float4(hit_information.origin,0));
  uint4 return_int = {100-buffer_value.y,100-buffer_value.y,100-buffer_value.y,0};
  //uint4 return_int = {current_ray.direction.x*128,current_ray.direction.y*128,current_ray.direction.z*128,0};
  return return_int*2;
}

__kernel void render(__write_only image2d_t frame, __read_only image3d_t reference_volume, __read_only image3d_t sdf, __global char* buffer_volume, float cam_pos_x, float cam_pos_y, float cam_pos_z, float cam_dir_x, float cam_dir_y, float cam_dir_z, int random_seed){
  unsigned int x = get_global_id(0);
  unsigned int y = get_global_id(1);
  int2 pos = {x, y};
  int3 refdimensions = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};

  write_imageui(frame, pos, (uint4){0,0,0,0});

  struct ray camera = {{cam_pos_x, cam_pos_y, cam_pos_z}, {cam_dir_x, cam_dir_y, cam_dir_z}};
  //struct ray camera = {{-100,220,-100},{-0.707,0,-0.707}};
  struct ray vray = generate_ray(camera, x,y,get_image_width(frame), get_image_height(frame));
  struct cut_result cut_result;

  if (in_volume(reference_volume, vray.origin) == false)
    cut_result = cut(reference_volume, vray);
  else {
    struct cut_result res = {true, vray.origin};
    cut_result = res;
  }

  if(!cut_result.cut) return;

  struct ray surface_ray = {cut_result.cut_point, vray.direction};

  uint4 color = {cut_result.cut_point.x, cut_result.cut_point.y, cut_result.cut_point.z,255};
  color /= 16;

  uint4 f = compute_ao(surface_ray, reference_volume, sdf, buffer_volume, random_seed);
  color = f;

  if(color.x + color.y + color.z < 255*3)
  write_imageui(frame, pos, color);
}
