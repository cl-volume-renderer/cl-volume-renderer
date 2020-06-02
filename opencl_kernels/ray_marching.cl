__kernel bool first_cut(float3 pos, float3 dir, float3 *entry){

}

__kernel create_ray(int x_coordinate, int y_coordinate, float3 *direction){

}


__kernel render(__write_only image2d_t frame, __read_only image3d_t reference_volume, /*read_and_write*/ image3d_t buffer_volume, float cam_pos_x, float cam_pos_y, float cam_pos_z, float cam_dir_x, float cam_dir_y, float cam_dir_z){

}
