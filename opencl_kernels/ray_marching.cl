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

  const float x_f = x;
  const float y_f = y;
  
  const float3 plane_middle = camera.origin + camera.direction;
  const float aspect_ratio = (float)x_total / (float)y_total;
  const float x_offset = x_f / x_total * aspect_ratio;
  const float y_offset = y_f / y_total;

  const float3 point_on_plane = normalize(plane_middle + x_offset*cam_side + y_offset*up);
  const struct ray ret = {camera.origin, point_on_plane};
  return ret;
}

__kernel void render(__write_only image2d_t frame, __read_only image3d_t reference_volume, /*read_and_write*/ image3d_t buffer_volume, float cam_pos_x, float cam_pos_y, float cam_pos_z, float cam_dir_x, float cam_dir_y, float cam_dir_z){
  unsigned int x = get_global_id(0);
  unsigned int y = get_global_id(1);
  int2 pos = {x, y};

  struct ray camera = {{-100+cam_dir_x,0+cam_dir_y,-100+cam_dir_z},{-0.707+cam_pos_y,0,-0.707+cam_pos_x}};
  struct ray vray = generate_ray(camera, x,y,get_image_width(frame), get_image_height(frame));
  struct cut_result cut_result = cut(reference_volume, vray);
  uint4 color = {cut_result.cut_point.x, cut_result.cut_point.y, cut_result.cut_point.z,255};

  write_imageui(frame, pos, color);
}
