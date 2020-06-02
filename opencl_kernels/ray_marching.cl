struct ray {
  float3 origin;
  float3 direction;
};

struct cut_result{
  bool cut;
  float3 cut_point;
};

struct cut_result cut(__read_only image3d_t reference_volume, struct ray shot) {
   struct cut_result res = {false, {0.0, 0.0, 0.0}};



   return res;
}

__kernel void render(__write_only image2d_t frame, __read_only image3d_t reference_volume, /*read_and_write*/ image3d_t buffer_volume, float cam_pos_x, float cam_pos_y, float cam_pos_z, float cam_dir_x, float cam_dir_y, float cam_dir_z){
  unsigned int x = get_global_id(0);
  unsigned int y = get_global_id(1);
  int2 pos = {x, y};
  uint4 color = {255, 0, 0, 255};
  write_imageui(frame, pos, color);
}
