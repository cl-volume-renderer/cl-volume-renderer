struct ray {
  float3 origin;
  float3 direction;
};

struct cut_result{
  bool cut;
  float3 cut_point;
};

#define MINIMUM_CUT(field) min((refdimensions.field - shot.origin.field) / shot.direction.field, (-shot.origin.field) / shot.direction.field)
#define CALC_CUT_POINT(field) shot.origin + field * shot.direction;
#define LIMITS(point, field) (point.field <= refdimensions.field && point.field >= 0)

struct cut_result cut(__read_only image3d_t reference_volume, struct ray shot) {
  struct cut_result res = {false, {0.0, 0.0, 0.0}};
  int3 refdimensions = {get_image_width(reference_volume), get_image_height(reference_volume), get_image_depth(reference_volume)};

  //we first calc the cut points of 3 planes. Each plane is defined by 0 and the max size of the dimension
  float t_x = MINIMUM_CUT(x);
  float3 x_cut_point = CALC_CUT_POINT(t_x)

  float t_y = MINIMUM_CUT(y);
  float3 y_cut_point = CALC_CUT_POINT(t_y)

  float t_z = MINIMUM_CUT(z);
  float3 z_cut_point = CALC_CUT_POINT(t_z)

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
  const float3 cam_dir = camera.direction;
  const float3 cam_side = normalize(cross(up,cam_dir));

  const float x_f = x - x_total/2;
  const float y_f = y - y_total/2;
  
  const float3 plane_middle = camera.direction;
  const float aspect_ratio = (float)x_total / (float)y_total;
  const float x_offset = x_f / x_total * aspect_ratio;
  const float y_offset = y_f / y_total;

  const float3 point_on_plane = plane_middle + x_offset*cam_side + y_offset*up;
  const struct ray ret = {camera.origin, normalize(point_on_plane)};
  return ret;
}

__kernel void render(__write_only image2d_t frame, __read_only image3d_t reference_volume, /*read_and_write*/ image3d_t buffer_volume, float cam_pos_x, float cam_pos_y, float cam_pos_z, float cam_dir_x, float cam_dir_y, float cam_dir_z){
  unsigned int x = get_global_id(0);
  unsigned int y = get_global_id(1);
  int2 pos = {x, y};

  write_imageui(frame, pos, (uint4){0,0,0,0}); 

  struct ray camera = {{cam_pos_x, cam_pos_y, cam_pos_z}, {cam_dir_x, cam_dir_y, cam_dir_z}};
  //struct ray camera = {{-100,220,-100},{-0.707,0,-0.707}};
  struct ray vray = generate_ray(camera, x,y,get_image_width(frame), get_image_height(frame));
  struct cut_result cut_result = cut(reference_volume, vray);

   if(!cut_result.cut) return;

  struct ray surface_ray = {cut_result.cut_point, vray.direction};
  float step_size = 0.5;
  float acc_value = 0;
  for(int i = 0; i < 400; ++i){
    int location_x = surface_ray.origin.x + step_size*surface_ray.direction.x*i;
    int location_y = surface_ray.origin.y + step_size*surface_ray.direction.y*i;
    int location_z = surface_ray.origin.z + step_size*surface_ray.direction.z*i;

    int4 location = {location_x, location_y, location_z, 0};
    int4 value = read_imagei(reference_volume, CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP, location);
    if(value.x > 0 && value.x < 255)
    //if(value.x > 800)
    acc_value += (value.x); 

  }
  acc_value = (acc_value / 100);
  
  uint4 color = {cut_result.cut_point.x, cut_result.cut_point.y, cut_result.cut_point.z,255};
  color /= 16;
  color.x += acc_value;
  color.y += acc_value;
  color.z += acc_value;

  write_imageui(frame, pos, color); 
}
