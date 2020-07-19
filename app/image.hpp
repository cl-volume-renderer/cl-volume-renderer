#pragma once
#include <vector>

//An image with a fixed with and height
struct image {
  std::vector<unsigned char> m_pixels;
  const unsigned int m_width;
  const unsigned int m_height;
  const unsigned int m_pixel_depth;

  ///Create an image using r-value reference to data vector
  ///@param width the width of the image
  ///@param height the width of the image
  ///@param pixel_depth the width of the image
  image(std::vector<unsigned char>&& input_vector, const unsigned int width, const unsigned int height, const unsigned int pixel_depth);
};
