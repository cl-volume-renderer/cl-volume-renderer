#include "hdre_loader.h"
#include <stb_image.h>
#include <vector>
#include <iostream>

image hdre_loader::load_file(std::string path) {
  int width, height, n;
  stbi_info(path.data(), &width, &height, &n);

  unsigned char *data = stbi_load(path.data(), &width, &height, &n, 0);
  if(data == NULL){
    std::cerr << "Error, failed to load file: " + path << "\n";
    exit(1);
  
  }
  std::vector<unsigned char> image_data(data, data + width*height*n);
  delete[] data;
  return image(std::move(image_data), width, height, n);
}


