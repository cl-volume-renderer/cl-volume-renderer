#define MAX_STEP_COUNT 1000
#clw_include_once "utility.cl"
#clw_include_once "utility_ray.cl"
#clw_include_once "utility_sampling.cl"
#clw_include_once "utility_filter.cl"


uint4 compute_light(struct ray surface_ray, __read_only image3d_t reference_volume, __read_only image3d_t sdf, __global short* buffer_volume, int random_seed){
  const sampler_t smp = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
  const int3 reference_dimensions = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};

  float hit_accumulator = 0.0;

  struct ray current_ray = surface_ray;
  struct ray hit_information = {0};
  int4 buffer_value = {0,0,0,0};

  enum event ray_event;
  current_ray = march_to_next_event(surface_ray, reference_volume, sdf, &ray_event);
  if(ray_event == Hit){
    buffer_value = buffer_volume_read4f(reference_dimensions, buffer_volume, make_float4(current_ray.origin,0));
    hit_information = current_ray;
    //Compute AO further?
    if(buffer_value.w < 500){
      buffer_value.w = buffer_value.w + 1;

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
      if(ray_event == Exit_volume){
        float  sun_distance = 2;
        float3 sun_dir = {0.0f, 1.0f, 0.0f};
        float4 sun = {1.0f,0.95f,0.8f,0.0f};
        
        float key_distance = 6;
        float3 key_dir = {1.0f, 0.0f, 0};
        float4 key = {1.f, 1.f, 1.f, 0.0f};

        float sec_distance = 1;
        float3 sec_dir = {0.707f, 0.0f, 0.707f};
        float4 sec = {0.05f, 0.05f, 0.3f, 0.0f};

        float sun_cont = pow(clamp(dot(current_ray.direction, normalize(sun_dir)), 0.f, 1.f), sun_distance) * 127;
        float key_cont = pow(clamp(dot(current_ray.direction, normalize(key_dir)), 0.f, 1.f), key_distance) * 127;
        float sec_cont = pow(clamp(dot(current_ray.direction, normalize(sec_dir)), 0.f, 1.f), sec_distance) * 127;

        float4 si = sun_cont * sun;
        float4 ki = key_cont * key;
        float4 sei = sec_cont * sec;
        buffer_value.x += si.x + ki.x + sei.x;
        buffer_value.y += si.y + ki.y + sei.y;
        buffer_value.z += si.z + ki.z + sei.z;
      }
      buffer_volume_write4f(reference_dimensions, buffer_volume, make_float4(hit_information.origin, 0), buffer_value);
    }
  }else{
    return 0.0;
  }


  buffer_value = buffer_volume_read4f(reference_dimensions, buffer_volume, make_float4(hit_information.origin,0));
  //buffer_value /= buffer_value.w;
  buffer_value /= buffer_value.w;
  uint4 return_int = {buffer_value.x,buffer_value.y,buffer_value.z,0};
  //uint4 return_int = {current_ray.direction.x*128,current_ray.direction.y*128,current_ray.direction.z*128,0};
  return return_int*2;
 
}

//Expect the surface ray as entry point
uint4 compute_ao(struct ray surface_ray, __read_only image3d_t reference_volume, __read_only image3d_t sdf, __global short* buffer_volume, int random_seed){
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

__kernel void render(__write_only image2d_t frame, __read_only image3d_t reference_volume, __read_only image3d_t sdf, __global short* buffer_volume, float cam_pos_x, float cam_pos_y, float cam_pos_z, float cam_dir_x, float cam_dir_y, float cam_dir_z, int random_seed){
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

  uint4 f = compute_light(surface_ray, reference_volume, sdf, buffer_volume, random_seed);
  color = f;

  //if(color.x + color.y + color.z < 255*3)
  write_imageui(frame, pos, color);
}
