#clw_include_once "utility_sampling.cl"
#clw_include_once "utility_filter.cl"

#define MIN_VALUE 0
#define MAX_VALUE 1
#define MIN_GRADIENT 2
#define MAX_GRADIENT 3
#define MAX_COUNT_VALUE 4

__kernel void fetch_stats(__read_only image3d_t reference_volume, __global int *stats)
{
  const sampler_t smp = CLK_FILTER_LINEAR;

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
