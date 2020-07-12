float gauss(float a, float sigma){
  return a / (2*sigma*sigma);
}

//Applies a 2D bilateral filter (not tested)
__kernel void bilateral_filter(__read_write image2d_t frame, int kernel_size, float sigma){
  const int2 pos = {get_global_id(0),get_global_id(1)};
  const sampler_t smp = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
  const uint4 ref_colour = read_imageui(frame, smp, pos);

  float Wpr = 0.0f;
  float Wpg = 0.0f;
  float Wpb = 0.0f;

  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;

  for(int x = -kernel_size; x <= kernel_size; ++x){
    for(int y = -kernel_size; y <= kernel_size; ++y){
      if(x == 0 && y == 0) continue;
      int2 off = {x,y};
      const uint4 read_colour = read_imageui(frame, smp, pos + off);
      float g1 = gauss(pow((float)(pos.x - off.x),2) + pow((float)(pos.y - off.y), 2),sigma);
      float g2r = gauss(fabs((float)(read_colour.x - ref_colour.x)),sigma);
      float g2g = gauss(fabs((float)(read_colour.y - ref_colour.y)),sigma);
      float g2b = gauss(fabs((float)(read_colour.z - ref_colour.z)),sigma);
      r += ref_colour.x *g1 * g2r;
      g += ref_colour.y *g1 * g2g;
      b += ref_colour.z *g1 * g2b;
      Wpr += g1*g2r;
      Wpg += g1*g2g;
      Wpb += g1*g2b;
    }
  }

  r = r/Wpr;
  g = g/Wpr;
  b = b/Wpr;
  uint4 color = {r,g,b,0};

  write_imageui(frame, pos, color);
}
