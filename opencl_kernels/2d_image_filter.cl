float gauss(float a, float sigma){
  return a / (2*sigma*sigma);
}

__kernel void bilateral_filter(__write_write image2d_t frame, int kernel_size, float sigma){
  const int2 pos = {get_global_id(0),get_global_id(1)};
  const sampler_t smp = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
  const uint4 ref_colour = read_imageui(frame, smp, pos + off);

  float Wpr = 0.0f;
  float Wpg = 0.0f;
  float Wpb = 0.0f;

  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;

  for(int x = -kernel_size; x <= kernel_size; ++x){
    for(int y = -kernel_size; y <= kernel_size; ++y){
      if(x == 0 && y == 0) continue;
      uint2 off = {x,y};
      const uint4 read_colour = read_imageui(frame, smp, pos + off);
      float g1 = gauss(pow((pos.x - off.x),2) + pow(pos.y - off.y, 2),sigma);
      float g2r = gauss(abs(read_colour.r - ref_colour.r),sigma);
      float g2g = gauss(abs(read_colour.g - ref_colour.g),sigma);
      float g2b = gauss(abs(read_colour.b - ref_colour.b),sigma);
      r += ref_colour.r *g1 * g2r;
      g += ref_colour.g *g1 * g2g;
      b += ref_colour.b *g1 * g2b;
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
