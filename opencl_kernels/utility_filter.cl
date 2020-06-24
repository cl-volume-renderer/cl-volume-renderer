float3 gradient_prewitt_nn(__read_only image3d_t reference_volume, float4 position){
  const float4 p = position;
  const sampler_t sampler = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
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
  dz += read_imagei(reference_volume, sampler, p + z + x).x;
  //dz += read_imagei(reference_volume, sampler, p + z - x).x;
  //dz += read_imagei(reference_volume, sampler, p + z    ).x;
  //dz -= read_imagei(reference_volume, sampler, p - z + x).x;
  //dz -= read_imagei(reference_volume, sampler, p - z - x).x;
  dz -= read_imagei(reference_volume, sampler, p - z    ).x;

  float3 return_float = {dx,dy,dz};
  return return_float;
}
