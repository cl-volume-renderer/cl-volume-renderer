#define MAX_STEP_COUNT 1000

struct ray {
  float3 origin;
  float3 direction;
};

struct cut_result{
  bool cut;
  float3 cut_point;
};

struct line_cut_result {
  bool already_passed;
  float line_t;
};

inline struct line_cut_result
cut_min_eval(float a, float b){
  struct line_cut_result lc = {false, 0};

  if (a <= 0 || b <= 0) {
    return lc;
  } else {
    lc.already_passed = true;
    lc.line_t = min(a, b);
  }

  return lc;
}

inline float4 make_float4 /*great_again*/(float3 value, float other){
  const float4 return_float = {value.x, value.y, value.z, other};
  return return_float;
}

inline int4 make_int(float4 value){
  const int4 return_int = {value.x, value.y, value.z, value.z};
  return return_int;
}

#define MINIMUM_CUT(field) cut_min_eval((refdimensions.field - shot.origin.field) / shot.direction.field, (-shot.origin.field) / shot.direction.field)
#define CALC_CUT_POINT(field) shot.origin + field * shot.direction;
#define LIMITS(point, field) (point.field <= refdimensions.field && point.field >= 0)

struct cut_result cut(__read_only image3d_t reference_volume, struct ray shot) {
  struct cut_result res = {false, {0.0, 0.0, 0.0}};
  int3 refdimensions = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};

  //we first calc the cut points of 3 planes. Each plane is defined by 0 and the max size of the dimension
  struct line_cut_result t_x = MINIMUM_CUT(x);
  float3 x_cut_point = CALC_CUT_POINT(t_x.line_t)

  struct line_cut_result t_y = MINIMUM_CUT(y);
  float3 y_cut_point = CALC_CUT_POINT(t_y.line_t)

  struct line_cut_result t_z = MINIMUM_CUT(z);
  float3 z_cut_point = CALC_CUT_POINT(t_z.line_t)

  //check if a cut of axis is within the boundaries of the other 2 axis. If so, we have a cut.
  if (LIMITS(x_cut_point, y) && LIMITS(x_cut_point, z)) {
    res.cut = true;
    res.cut_point = x_cut_point;
  }
  if (LIMITS(y_cut_point, x) && LIMITS(y_cut_point, z)) {
    res.cut = true;
    res.cut_point = y_cut_point;
  }
  if (LIMITS(z_cut_point, x) && LIMITS(z_cut_point, y)) {
    res.cut = true;
    res.cut_point = z_cut_point;
  }

  return res;
}

struct ray generate_ray(struct ray camera, int x, int y, int x_total, int y_total){
  const float3 up = {0.0, 1.0, 0.0};
  const float3 cam_dir  = camera.direction;
  const float3 cam_side = normalize(cross(up,cam_dir));
  float3 cam_up   = normalize(cross(cam_dir,cam_side));
  if(cam_up.y < 0){
    cam_up = -cam_up;
  }

  const float x_f = x - x_total/2;
  const float y_f = y - y_total/2;

  const float3 plane_middle = camera.direction;
  const float aspect_ratio = (float)x_total / (float)y_total;
  const float x_offset = x_f / x_total * aspect_ratio;
  const float y_offset = y_f / y_total;

  const float3 point_on_plane = plane_middle + x_offset*cam_side + y_offset*cam_up;
  const struct ray ret = {camera.origin, normalize(point_on_plane)};
  return ret;
}

bool in_volume(__read_only image3d_t reference_volume, float3 point) {
  int3 refdimensions = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};
  if (LIMITS(point, x) && LIMITS(point, y) && LIMITS(point, z))
    return true;
  return false;
}


float3 gradient_prewitt(__read_only image3d_t reference_volume, float4 position){
  const float4 p = position;
	const sampler_t sampler = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
  const float4 x = {1,0,0,0};
  const float4 y = {0,1,0,0};
  const float4 z = {0,0,1,0};
  int dx = 0;
  dx += read_imagei(reference_volume, sampler, p + x + y).x;
  dx += read_imagei(reference_volume, sampler, p + x - y).x;
  dx += read_imagei(reference_volume, sampler, p + x    ).x;
  dx -= read_imagei(reference_volume, sampler, p - x + y).x;
  dx -= read_imagei(reference_volume, sampler, p - x - y).x;
  dx -= read_imagei(reference_volume, sampler, p - x    ).x;

	int dy = 0;
  dy += read_imagei(reference_volume, sampler, p + y + x).x;
  dy += read_imagei(reference_volume, sampler, p + y - x).x;
  dy += read_imagei(reference_volume, sampler, p + y    ).x;
  dy -= read_imagei(reference_volume, sampler, p - y + x).x;
  dy -= read_imagei(reference_volume, sampler, p - y - x).x;
  dy -= read_imagei(reference_volume, sampler, p - y    ).x;


	int dz = 0;
  dz += read_imagei(reference_volume, sampler, p + z + x).x;
  dz += read_imagei(reference_volume, sampler, p + z - x).x;
  dz += read_imagei(reference_volume, sampler, p + z    ).x;
  dz -= read_imagei(reference_volume, sampler, p - z + x).x;
  dz -= read_imagei(reference_volume, sampler, p - z - x).x;
  dz -= read_imagei(reference_volume, sampler, p - z    ).x;

	float3 return_float = {dx,dy,dz};
	return normalize(return_float);
}


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



int2 buffer_volume_read(int3 refdimensions, __global char *buffer_volume, int4 pos)
{
   int2 ret = {0, 0};

   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*2;

   ret.x = buffer_volume[idx + 0];
   ret.y = buffer_volume[idx + 1];

   return ret;
}
int2 buffer_volume_readf(int3 refdimensions, __global char *buffer_volume, float4 pos){
  return buffer_volume_read(refdimensions, buffer_volume, make_int(pos));
}


void buffer_volume_write(int3 refdimensions, __global char *buffer_volume, int4 pos, int2 valueb)
{
   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*2;

   buffer_volume[idx + 0] = valueb.x;
   buffer_volume[idx + 1] = valueb.y;
}
void buffer_volume_writef(int3 refdimensions, __global char *buffer_volume, float4 pos, int2 valueb){
  buffer_volume_write(refdimensions, buffer_volume, make_int(pos), valueb);
}

struct ray ray_bounce(struct ray current_ray, float3 normal, int seed){
  const struct ray bounced_ray = {current_ray.origin + current_ray.direction, get_hemisphere_direction(normal, seed)};
  return bounced_ray;
}

inline bool exited_volume(__read_only image3d_t reference_volume, float4 position){
  const int3 ref_dimensions = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};
  return ref_dimensions.x < position.x | ref_dimensions.y < position.y | ref_dimensions.z < position.z;
}

#define EVENT_HIT
#define EVENT_EXIT_VOLUME

enum event{
  None,
  Hit,
  Exit_volume
};

inline bool is_event(short value){
  if(value > 800)
    return true;
  return false;
}

inline enum event get_event_and_value(__read_only image3d_t reference_volume, float4 position, int4* value_at_event){
  if(exited_volume(reference_volume, position))
    return Exit_volume;

  const sampler_t smp = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
  *value_at_event = read_imagei(reference_volume, smp, position);
  if(is_event(value_at_event->x))
    return Hit;
  return None;
}

inline enum event get_event(__read_only image3d_t reference_volume, float4 position){
  int4 value_at_event;
  return get_event_and_value(reference_volume, position, &value_at_event); 

}

struct ray march(struct ray current_ray, __read_only image3d_t sdf){
  const sampler_t smp = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
  float signed_distance = read_imagei(sdf, smp, make_float4(current_ray.origin,0)).x;
  const float step_size = max(signed_distance*0.9f, 0.5f);//Choose this dyn. later
  const struct ray return_ray = {current_ray.origin + step_size*current_ray.direction, current_ray.direction};
  return return_ray;
}

struct ray march_to_next_event(struct ray current_ray, __read_only image3d_t reference_volume, __read_only image3d_t sdf, enum event *event_type){
  enum event internal_event = None;
  for(int i = 0; i < 70; ++i){
    current_ray = march(current_ray, sdf);  
    internal_event = get_event(reference_volume, make_float4(current_ray.origin,0)); 
    if(internal_event != None){
      break;
    }
  }
  *event_type = internal_event;
  return current_ray;
}

//Expect the surface ray as entry point
uint4 compute_ao(struct ray surface_ray, __read_only image3d_t reference_volume, __read_only image3d_t sdf, __global char* buffer_volume, int random_seed){
  const sampler_t smp = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
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
      
      current_ray = ray_bounce(current_ray, -gradient_prewitt(reference_volume, make_float4(current_ray.origin,0)),random_seed); 
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
