#clw_include_once "utility_sampling.cl"
#clw_include_once "utility_filter.cl"
#define MIN_VALUE 0
#define MAX_VALUE 1
#define MIN_GRADIENT 2
#define MAX_GRADIENT 3
#define MAX_COUNT_VALUE 4

__kernel void tf_fetch_min_max(__read_only image3d_t reference_volume, __global int *stats)
{
  const sampler_t smp = CLK_FILTER_NEAREST;

  float4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
  int ref_value = read_imagei(reference_volume, smp, pos).x;
  float grad_length = length(gradient_prewitt_nn(reference_volume, pos));
  int4 reference_size = get_image_dim(reference_volume);

  if (pos.x >= reference_size.x || pos.y >= reference_size.y || pos.z >= reference_size.z)
    return;

  atomic_min(&stats[MIN_VALUE], ref_value);
  atomic_max(&stats[MAX_VALUE], ref_value);
  atomic_min(&stats[MIN_GRADIENT], grad_length);
  atomic_max(&stats[MAX_GRADIENT], grad_length);
}

__kernel void tf_sort_values(__read_only image3d_t reference_volume, __global uint *frame,
  int width, int height, __global int *stats)
{
   const sampler_t smp = CLK_FILTER_NEAREST;

  float4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
  int ref_value = read_imagei(reference_volume, smp, pos).x;
  float grad_length = length(gradient_prewitt_nn(reference_volume, pos));
  int4 reference_size = get_image_dim(reference_volume);

  int min_value = stats[MIN_VALUE];
  int max_value = stats[MAX_VALUE];
  int min_gradient = stats[MIN_GRADIENT];
  int max_gradient = stats[MAX_GRADIENT];

   if (pos.x >= reference_size.x || pos.y >= reference_size.y || pos.z >= reference_size.z)
     return;

  float value_range = max_value - min_value;
  float gradiant_range = max_gradient - min_gradient;
  int2 relative_point = {
    round(((float)(ref_value - min_value)/value_range)*width),
    round(((float)(grad_length - min_gradient)/gradiant_range)*height)
  };
  int a = atomic_inc(&frame[relative_point.x * height + relative_point.y]);
  atomic_max(&stats[MAX_COUNT_VALUE], a);
}

__kernel void tf_flush_color_frame(__write_only image2d_t color_frame,
  __global int *frame, __global int *lookup, int lookup_len, __global int *stats)
{
   const sampler_t smp = CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;

  int2 pos = {get_global_id(0), get_global_id(1)};
  int2 frame_size = get_image_dim(color_frame);
  int max_count_value = stats[MAX_COUNT_VALUE];

  if (pos.x >= frame_size.x || pos.y >= frame_size.y)
   return;

  int value = frame[get_global_id(0)*frame_size.y + (frame_size.y - get_global_id(1) - 1)];
  int local_value = -1;
  for(int i = 0; i < lookup_len; i++) {
    if (lookup[i] == value) {
      local_value = i;
      break;
    }
  }

  //move the color of all elements from 0 to max_count value to min_color to max_color. This way you can also see all the little details
  float min_color = 20.0f;
  float max_color = 255.0f;

  int result = 0;
  if (local_value > -1)
    result = min_color + (
      ((float)local_value) / (float)lookup_len)
          *(max_color - min_color);

  int4 value_color = {result, result, result, 255};

  write_imagei(color_frame, pos, value_color);
}
