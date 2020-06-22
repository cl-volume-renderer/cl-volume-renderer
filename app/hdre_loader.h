#pragma once
#include <vector>
#include <string>
#include "image.h"

/// Header for hdrE file, only contains relevant information.
struct hdre_header {
  unsigned int x;
  unsigned int y;
};

/// Load hdrE files. Limited to following:
///   https://en.wikipedia.org/wiki/RGBE_image_format
class hdre_loader {
 public:
  /// Loads hdrE file into memory
  ///@return an image object containing the data from path
  ///@param path the path to load hdrE file from
  image load_file(const std::string path);
};
