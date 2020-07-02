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
    std::array<size_t, 3> volume_size;
    clw_image<const short> volume; //image data input in 3D
    std::array<int, 2> value_range;
    std::array<int, 2> gradient_range;
    std::array<int, 2> value_clip = {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
    std::array<int, 2> gradient_clip = {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
  public:
    reference_volume(clw_context &ctx, volume_block *b) :
                         volume_size({(unsigned int)b->m_voxel_count_x, (unsigned int)b->m_voxel_count_y, (unsigned int)b->m_voxel_count_z}),
                         volume(ctx, std::move(b->m_voxels), volume_size, true),
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
        {4,4,4}, volume,
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

    std::array<int, 2> get_value_range() const {
      return {std::max(value_clip[0], value_range[0]), std::min(value_clip[1], value_range[1])};
    }

    std::array<int, 2> get_gradient_range() const {
      return {std::max(gradient_clip[0], gradient_range[0]), std::min(gradient_clip[1], gradient_range[1])};
    }

    const std::array<size_t, 3>& get_volume_size() const {
      return volume_size;
    }

    std::array<size_t, 3> get_volume_size_evenness(unsigned int l) const{
      return {evenness(volume_size[0], l), evenness(volume_size[1], l), evenness(volume_size[2], l)};
    }

    size_t get_volume_length() const {
      return volume_size[0] * volume_size[1] * volume_size[2];
    }
    const clw_image<const short>& get_reference_volume() const {
      return volume;
    }
    Volume_Stats get_volume_stats()  const {
      return Volume_Stats(get_value_range(), get_gradient_range());
    }
};
