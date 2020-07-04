#pragma once

#include <limits>
#include <clw_image.h>
#include <clw_context.h>
#include <clw_vector.h>
#include <clw_function.h>
#include "volume_block.h"
#include "common.h"
#include "debug_helper.h"

struct Volume_Stats{
  float min_v;
  float max_v;
  float min_g;
  float max_g;
  template <typename T>
  Volume_Stats(const T& value_stats, const T& gradient_stats) {
    min_v = value_stats[0];
    max_v = value_stats[1];
    min_g = gradient_stats[0];
    max_g = gradient_stats[1];
  }
  Volume_Stats() {
    min_v = max_v = min_g = max_g = 0;
  }
};

class reference_volume {
  private:
    clw_context &ctx;
    std::array<size_t, 3> volume_size;
    std::array<size_t, 3> cropped_volume_size;
    std::array<size_t, 3>clip_min, clip_max;
    clw_image<short> original_volume; //image data input in 3D, uncropped
    clw_image<short> cropped_volume; //image data cropped from original volume
    std::array<int, 2> value_range;
    std::array<int, 2> gradient_range;
    std::array<int, 2> value_clip = {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
    std::array<int, 2> gradient_clip = {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
  public:
    reference_volume(clw_context &c, volume_block *b) :
                         ctx(c),
                         volume_size({(unsigned int)b->m_voxel_count_x, (unsigned int)b->m_voxel_count_y, (unsigned int)b->m_voxel_count_z}),
                         cropped_volume_size(volume_size),
                         original_volume(ctx, std::move(b->m_voxels), volume_size, true),
                         cropped_volume(ctx, std::vector<short>(8), {2, 2, 2}, false),
                         value_range({0, 0}),
                         gradient_range({0, 0}) {
      TIME_START();
      //{
      //  clw_image<short> buffer(ctx, std::vector<short> (b->m_voxel_size_x*b->m_voxel_count_y*b->m_voxel_count_z, 0), volume_size, false);
      //  auto bilateral_filter = clw_function(ctx, "volume_filter.cl", "bilateral_filter");
      //  bilateral_filter.execute(get_volume_size_evenness(8),{4,4,4}, volume, buffer);
      //  volume = std::move(buffer);//Doesn't work, because the reference_volume is const
      //}


      //fetch stats
      std::vector<int> stats_buffer {
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::min()};
      clw_vector<int> stats(ctx, std::move(stats_buffer), true);

      auto tf_min_max_construction = clw_function(ctx, "reference_volume_figures.cl", "fetch_stats");
      tf_min_max_construction.execute(
        get_volume_size_evenness(8),
        {4,4,4}, original_volume,
        stats);
      stats.pull();

      value_range[0] = stats[0];
      value_range[1] = stats[1];
      gradient_range[0] = stats[2];
      gradient_range[1] = stats[3];
      std::cout << gradient_range[1];
      TIME_PRINT("stats fetch time");
    }

    void set_value_clip(std::array<int, 2> clip) {
      value_clip = clip;
    }

    void set_gradient_clip(std::array<int, 2> clip) {
      gradient_clip = clip;
    }

    void set_clipping(std::array<size_t, 3> min, std::array<size_t, 3> max) {
      clip_min = min;
      clip_max = max;
      assert(clip_min[0] < clip_max[0]);
      assert(clip_min[1] < clip_max[1]);
      assert(clip_min[2] < clip_max[2]);
      cropped_volume_size = {clip_max[0] - clip_min[0], clip_max[1] - clip_min[1], clip_max[2] - clip_min[2]};
      clw_vector<unsigned int> start(ctx, std::vector<unsigned int>({(unsigned int)clip_min[0], (unsigned int)clip_min[1], (unsigned int)clip_min[2]}), true);
      clw_vector<unsigned int> length(ctx, std::vector<unsigned int>({(unsigned int)(clip_max[0] - clip_min[0]), (unsigned int)(clip_max[1] - clip_min[1]), (unsigned int)(clip_max[2] - clip_min[2]), 4}), true);
      auto copy_func = clw_function(ctx, "reference_volume_clip.cl", "apply_clip");
      cropped_volume = clw_image<short>(ctx, std::vector<short>(get_volume_length()), get_volume_size());

      copy_func.execute(get_volume_size_evenness(4), {4, 4, 4}, original_volume, cropped_volume, start, length);
    }

    std::array<int, 2> get_value_range() const {
      return {std::max(value_clip[0], value_range[0]), std::min(value_clip[1], value_range[1])};
    }

    std::array<int, 2> get_gradient_range() const {
      return {std::max(gradient_clip[0], gradient_range[0]), std::min(gradient_clip[1], gradient_range[1])};
    }
    const std::array<size_t, 3>& get_original_volume_size() const {
      return volume_size;
    }
    const std::array<size_t, 3>& get_volume_size() const {
      return cropped_volume_size;
    }

    std::array<size_t, 3> get_volume_size_evenness(unsigned int l) const{
      return {evenness(cropped_volume_size[0], l), evenness(cropped_volume_size[1], l), evenness(cropped_volume_size[2], l)};
    }

    size_t get_volume_length() const {
      return cropped_volume_size[0] * cropped_volume_size[1] * cropped_volume_size[2];
    }
    const clw_image<short>& get_reference_volume() const {
      if (cropped_volume.size() > 8)
        return cropped_volume;
      else
        return original_volume;
    }
    Volume_Stats get_volume_stats()  const {
      return Volume_Stats(get_value_range(), get_gradient_range());
    }
};
