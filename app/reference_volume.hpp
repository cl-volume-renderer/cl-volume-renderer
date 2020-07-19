#pragma once

#include <limits>
#include <clw_image.hpp>
#include <clw_context.hpp>
#include "volume_block.hpp"

struct Volume_Stats{
  float min_v; // minimum value in the volume
  float max_v; // maximum value in the volume
  float min_g; // minimum gradient in the volume
  float max_g; // maximum gradient in the volume
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
    std::array<size_t, 3> volume_size; //the original volume size
    std::array<size_t, 3> cropped_volume_size; //the size of the cropped volume
    std::array<size_t, 3>clip_min, clip_max; // the minimum and maximum coordinates for the cropped volume
    clw_image<short> original_volume; //image data input in 3D, uncropped
    clw_image<short> cropped_volume; //image data cropped from original volume
    std::array<int, 2> value_range; //The range of values in the volume data, always over the original volume
    std::array<int, 2> gradient_range; //The range of gradients in the volume data, always over the original volume
    std::array<int, 2> value_clip = {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}; //value clip, no value will always be above, or below the set min & max.
    std::array<int, 2> gradient_clip = {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}; //gradient clip, no gradient will always be above, or below the set min & max.
  public:
    reference_volume(clw_context &c, volume_block *b);
    ///set value clip, no value will be above the min & max, clip[0] is min clip[1] is max.
    void set_value_clip(std::array<int, 2> clip);
    ///set gradient clip, no gradient will be above the min & max, clip[0] is min clip[1] is max.
    void set_gradient_clip(std::array<int, 2> clip);
    ///clip the volume to the passed 3d points
    void set_clipping(std::array<size_t, 3> min, std::array<size_t, 3> max);
    ///Applies a filter (smoothing) to the reference volume or cropped reference volume
    void filter();
    //Get the range of value in the volume, this function does consider value_clip
    std::array<int, 2> get_value_range() const;
    ///Get the range of gradient in the volume, this function does consider gradient_clip
    std::array<int, 2> get_gradient_range() const;
    ///Get the size of the original size
    const std::array<size_t, 3>& get_original_volume_size() const;
    ///Get the size of the cropped volume
    const std::array<size_t, 3>& get_volume_size() const;
    ///get_volume_size, rounded up, so its devidable by l
    std::array<size_t, 3> get_volume_size_evenness(unsigned int l) const;
    ///get the length of the volume l*w*d
    size_t get_volume_length() const;
    ///Returns a clw_image that contains the volume, this considers the clipping points
    const clw_image<short>& get_reference_volume() const;
    ///Get the stats of the volume.
    Volume_Stats get_volume_stats()  const;
};
