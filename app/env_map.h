#pragma once

#include "hdre_loader.h"
#include "clw_image.h"

class env_map {
private:
  clw_image<unsigned char, 4> buffer;
public:
  env_map(clw_context &ctx, image &env_map) : buffer(ctx, std::move(env_map.m_pixels), {env_map.m_width, env_map.m_height}, true) {
  }
  clw_image<unsigned char, 4>& get_buffer() {
    return buffer;
  }
};
