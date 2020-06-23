#clw_include_once "utility_sampling.cl"

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


struct ray ray_bounce(struct ray current_ray, float3 normal, int seed){
  const struct ray bounced_ray = {current_ray.origin + current_ray.direction, get_hemisphere_direction(normal, seed)};
  return bounced_ray;
}

inline bool exited_volume(__read_only image3d_t reference_volume, float4 position){
  const int3 ref_dimensions = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};
  const bool exited_max = ref_dimensions.x < position.x | ref_dimensions.y < position.y | ref_dimensions.z < position.z;
  const bool exited_min = position.x < 0 | position.y < 0 | position.z < 0;
  return exited_max | exited_min;
}

enum event{
  None,
  Hit,
  Exit_volume
};

inline bool is_event(short value){
  return is_event_gen(value, 0.0);
  /*
  if(value > 800)
    return true;
  return false;*/
}

inline enum event get_event_and_value(__read_only image3d_t reference_volume, float4 position, int4* value_at_event){
  if(exited_volume(reference_volume, position))
    return Exit_volume;

  const sampler_t smp = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
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
