struct ray {
  float3 origin;
  float3 direction;
};

struct cut_result{
  bool cut;
  float3 cut_point;
};

#define MINIMUM_CUT(field) min(refdimensions.field - shot.origin.field / shot.direction.field, shot.origin.field / shot.direction.field)
#define CALC_CUT_POINT(field) shot.origin + field * shot.direction;
#define LIMITS(field) (x_cut_point.field <= refdimensions.field && x_cut_point.field >= 0)

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
  if (LIMITS(y) && LIMITS(z)) {
    res.cut = true;
    res.cut_point = x_cut_point;
  }
  if (LIMITS(x) && LIMITS(z)) {
    res.cut = true;
    res.cut_point = y_cut_point;
  }
  if (LIMITS(x) && LIMITS(y)) {
    res.cut = true;
    res.cut_point = z_cut_point;
  }

  return res;
}

__kernel void render(__write_only image2d_t frame, __read_only image3d_t reference_volume, /*read_and_write*/ image3d_t buffer_volume, float cam_pos_x, float cam_pos_y, float cam_pos_z, float cam_dir_x, float cam_dir_y, float cam_dir_z){
  unsigned int x = get_global_id(0);
  unsigned int y = get_global_id(1);
  int2 pos = {x, y};
  uint4 color = {255, 0, 0, 255};
  write_imageui(frame, pos, color);
}
