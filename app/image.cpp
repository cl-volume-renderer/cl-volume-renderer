#include "image.h"
image::image(std::vector<unsigned char>&& input_vector, const unsigned int width, const unsigned int height, const unsigned int pixel_depth): m_pixels(std::move(input_vector)), m_width(width), m_height(height), m_pixel_depth(pixel_depth){}
