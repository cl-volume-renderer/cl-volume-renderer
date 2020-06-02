__kernel void hello_world(__global unsigned char* in, int size)
{
  if(get_global_id(0) < size){
    in[get_global_id(0)] = in[get_global_id(0)] + 1;
  }
}
__kernel void render_pixels(__global uchar* out, int width, int height)
{
  unsigned int x = get_global_id(0);
  unsigned int y = get_global_id(1);
  if (x < width && y < height) {
    unsigned int point = (x + y*width)*4;
    if((x/5) % 2 == 0){
      out[point + 0] = 255;
      out[point + 1] = 0;
      out[point + 2] = 0;
      out[point + 3] = 255;
    } else {
      out[point + 0] = 0;
      out[point + 1] = 0;
      out[point + 2] = 0;
      out[point + 3] = 255;
    }
  }
}
