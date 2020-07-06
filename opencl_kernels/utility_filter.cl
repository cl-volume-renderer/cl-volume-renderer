float3 gradient_prewitt_nn(__read_only image3d_t reference_volume, float4 position){
  const float4 p = position;
  const sampler_t sampler = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
  const float4 x = {1,0,0,0};
  const float4 y = {0,1,0,0};
  const float4 z = {0,0,1,0};
  int dx = 0;
  //dx += read_imagei(reference_volume, sampler, p + x + y).x;
  //dx += read_imagei(reference_volume, sampler, p + x - y).x;
  dx += read_imagei(reference_volume, sampler, p + x    ).x;
  //dx -= read_imagei(reference_volume, sampler, p - x + y).x;
  //dx -= read_imagei(reference_volume, sampler, p - x - y).x;
  dx -= read_imagei(reference_volume, sampler, p - x    ).x;

  int dy = 0;
  //dy += read_imagei(reference_volume, sampler, p + y + x).x;
  //dy += read_imagei(reference_volume, sampler, p + y - x).x;
  dy += read_imagei(reference_volume, sampler, p + y    ).x;
  //dy -= read_imagei(reference_volume, sampler, p - y + x).x;
  //dy -= read_imagei(reference_volume, sampler, p - y - x).x;
  dy -= read_imagei(reference_volume, sampler, p - y    ).x;


  int dz = 0;
  //dz += read_imagei(reference_volume, sampler, p + z + x).x;
  //dz += read_imagei(reference_volume, sampler, p + z - x).x;
  dz += read_imagei(reference_volume, sampler, p + z    ).x;
  //dz -= read_imagei(reference_volume, sampler, p - z + x).x;
  //dz -= read_imagei(reference_volume, sampler, p - z - x).x;
  dz -= read_imagei(reference_volume, sampler, p - z    ).x;

  float3 return_float = {dx,dy,dz};
  return return_float;
}


short bilateral_kernel(__read_only image3d_t reference_volume, float4 position){
  const float4 p = position;
  const sampler_t sampler = CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;

  const float sigmas = 0.6f;//Kernel size 2
  const float sigmar = 1.0f;
	const int radius = 2;
	float out_colour = 0;
	float mid_colour = read_imagei(reference_volume, sampler, p).x;
	float wp = 0;

	for (int z = -radius; z <= radius; ++z)
		for (int y = -radius; y <= radius; ++y)
			for (int x = -radius; x <= radius; ++x){
        float local_colour = read_imagei(reference_volume, sampler, p + (float4)(x,y,z,0)).x;
        float posd = ((float)(x*x + y*y + z*z))/(2*sigmas*sigmas);
        float cold = (pow((float)(mid_colour - local_colour), 2.0f))/(2*sigmar*sigmar);
        float w = exp(-posd - cold);
        wp += w;
        out_colour += local_colour*w;
			}

  return out_colour/wp;

}
