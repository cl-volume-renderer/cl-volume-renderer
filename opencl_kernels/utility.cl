inline float4 make_float4 /*great_again*/(float3 value, float other){
  return (float4)(value, other);
}

inline int4 make_int(float4 value){
  const int4 return_int = {value.x, value.y, value.z, value.z};
  return return_int;
}

bool atomic_allow_write_max(int3 refdimensions, __global unsigned short *buffer_volume, int4 pos, unsigned int max){
   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*4;
   __global int* buffer = (__global int*) (buffer_volume + idx);
   short w = buffer_volume[idx + 3];
   if(w > max){
    return false;
   }
   int t = atomic_add(buffer+1, 0x00010000);
    if((t >> 16) < max) return true;
   atomic_sub(buffer+1, 0x00010000);
   return false;
}

bool atomic_allow_write_maxf(int3 refdimensions, __global unsigned short *buffer_volume, float4 pos, unsigned int max){
  return atomic_allow_write_max(refdimensions, buffer_volume, make_int(pos), max);
}

void atomic_buffer_volume_add4(int3 refdimensions, __global unsigned short *buffer_volume, int4 pos, uint4 valueb)
{
   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*4;
   __global int* buffer = (__global int*) (buffer_volume + idx);

   unsigned int r = valueb.x & 0x0000FFFF;
   unsigned int g = valueb.y & 0x0000FFFF;
   unsigned int b = valueb.z & 0x0000FFFF;
   unsigned int a = valueb.w & 0x0000FFFF;
   
   unsigned int low = r +  (g << 16);
   unsigned int high = b + (a << 16);
   
   atomic_add(buffer,   low);
   atomic_add(buffer+1, high);
}

inline bool allow_write_max(int3 refdimensions, __global unsigned short *buffer_volume, int4 pos, unsigned int max){
  int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*4;
  short w = buffer_volume[idx + 3];
  if(w > max){
    return false;
  }
  buffer_volume[idx + 3] += 1;
  return true;
}

bool allow_write_maxf(int3 refdimensions, __global unsigned short *buffer_volume, float4 pos, unsigned int max){
  return allow_write_max(refdimensions, buffer_volume, make_int(pos), max);
}

void atomic_buffer_volume_add4f(int3 refdimensions, __global unsigned short *buffer_volume, float4 pos, uint4 valueb){
  atomic_buffer_volume_add4(refdimensions, buffer_volume, make_int(pos), valueb);
}

void buffer_volume_add4(int3 refdimensions, __global unsigned short* buffer_volume, int4 pos, uint4 valueb){
  int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*4;
  buffer_volume[idx + 0] += valueb.x;
  buffer_volume[idx + 1] += valueb.y;
  buffer_volume[idx + 2] += valueb.z;
  buffer_volume[idx + 3] += valueb.w;
}

void buffer_volume_add4f(int3 refdimensions, __global unsigned short* buffer_volume, float4 pos, uint4 valueb){
  buffer_volume_add4(refdimensions, buffer_volume, make_int(pos), valueb);
}

uint4 buffer_volume_read4(int3 refdimensions, __global unsigned short *buffer_volume, int4 pos)
{
   uint4 ret = {0, 0, 0, 0};

   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*4;

   ret.x = buffer_volume[idx + 0];
   ret.y = buffer_volume[idx + 1];
   ret.z = buffer_volume[idx + 2];
   ret.w = buffer_volume[idx + 3];

   return ret;
}

void buffer_volume_write4(int3 refdimensions, __global unsigned short *buffer_volume, int4 pos, uint4 valueb)
{
   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*4;

   buffer_volume[idx + 0] = valueb.x;
   buffer_volume[idx + 1] = valueb.y;
   buffer_volume[idx + 2] = valueb.z;
   buffer_volume[idx + 3] = valueb.w;
}


uint4 buffer_volume_read4f(int3 refdimensions, __global unsigned short *buffer_volume, float4 pos){
  return buffer_volume_read4(refdimensions, buffer_volume, make_int(pos));
}

void buffer_volume_write4f(int3 refdimensions, __global unsigned short *buffer_volume, float4 pos, uint4 valueb){
  buffer_volume_write4(refdimensions, buffer_volume, make_int(pos), valueb);
}

uint2 buffer_volume_read(int3 refdimensions, __global unsigned short *buffer_volume, int4 pos)
{
   uint2 ret = {0, 0};

   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*2;

   ret.x = buffer_volume[idx + 0];
   ret.y = buffer_volume[idx + 1];

   return ret;
}
uint2 buffer_volume_readf(int3 refdimensions, __global unsigned short *buffer_volume, float4 pos){
  return buffer_volume_read(refdimensions, buffer_volume, make_int(pos));
}


void buffer_volume_write(int3 refdimensions, __global unsigned short *buffer_volume, int4 pos, uint2 valueb)
{
   int idx = (refdimensions.x*refdimensions.z * pos.y + refdimensions.x * pos.z + pos.x)*2;

   buffer_volume[idx + 0] = valueb.x;
   buffer_volume[idx + 1] = valueb.y;
}
void buffer_volume_writef(int3 refdimensions, __global unsigned short *buffer_volume, float4 pos, uint2 valueb){
  buffer_volume_write(refdimensions, buffer_volume, make_int(pos), valueb);
}
