#pragma once

#include <limits>
#include <clw_image.h>
#include <clw_context.h>
#include "volume_block.h"

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
    reference_volume(clw_context &c, volume_block *b);
    void set_value_clip(std::array<int, 2> clip);
    void set_gradient_clip(std::array<int, 2> clip);
    void set_clipping(std::array<size_t, 3> min, std::array<size_t, 3> max);
    std::array<int, 2> get_value_range() const;
    std::array<int, 2> get_gradient_range() const;
    const std::array<size_t, 3>& get_original_volume_size() const;
    const std::array<size_t, 3>& get_volume_size() const;
    std::array<size_t, 3> get_volume_size_evenness(unsigned int l) const;
    size_t get_volume_length() const;
    const clw_image<short>& get_reference_volume() const;
    Volume_Stats get_volume_stats()  const;
};
