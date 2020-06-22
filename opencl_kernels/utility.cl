inline float4 make_float4 /*great_again*/(float3 value, float other){
  return (float4)(value, other);
}

inline int4 make_int(float4 value){
  const int4 return_int = {value.x, value.y, value.z, value.z};
  return return_int;
}

int4 buffer_volume_read4(int3 refdimensions, __global short *buffer_volume, int4 pos)
{
   int4 ret = {0, 0, 0, 0};

   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*4;

   ret.x = buffer_volume[idx + 0];
   ret.y = buffer_volume[idx + 1];
   ret.z = buffer_volume[idx + 2];
   ret.w = buffer_volume[idx + 3];

   return ret;
}
void buffer_volume_write4(int3 refdimensions, __global short *buffer_volume, int4 pos, int4 valueb)
{
   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*4;

   buffer_volume[idx + 0] = valueb.x;
   buffer_volume[idx + 1] = valueb.y;
   buffer_volume[idx + 2] = valueb.z;
   buffer_volume[idx + 3] = valueb.w;
}

int4 buffer_volume_read4f(int3 refdimensions, __global short *buffer_volume, float4 pos){
  return buffer_volume_read4(refdimensions, buffer_volume, make_int(pos));
}

void buffer_volume_write4f(int3 refdimensions, __global short *buffer_volume, float4 pos, int4 valueb){
  buffer_volume_write4(refdimensions, buffer_volume, make_int(pos), valueb);
}

int2 buffer_volume_read(int3 refdimensions, __global short *buffer_volume, int4 pos)
{
   int2 ret = {0, 0};

   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*2;

   ret.x = buffer_volume[idx + 0];
   ret.y = buffer_volume[idx + 1];

   return ret;
}
int2 buffer_volume_readf(int3 refdimensions, __global short *buffer_volume, float4 pos){
  return buffer_volume_read(refdimensions, buffer_volume, make_int(pos));
}


void buffer_volume_write(int3 refdimensions, __global short *buffer_volume, int4 pos, int2 valueb)
{
   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*2;

   buffer_volume[idx + 0] = valueb.x;
   buffer_volume[idx + 1] = valueb.y;
}
void buffer_volume_writef(int3 refdimensions, __global short *buffer_volume, float4 pos, int2 valueb){
  buffer_volume_write(refdimensions, buffer_volume, make_int(pos), valueb);
}
