//TODO Move from private to cpp file
#pragma once

#include <string>
#include <vector>
#include "volume_block.hpp"
/// Header for NRRD file, only contains relevant information.
/// Notice, not all filters will use all settings
struct nrrd_header {
  unsigned int x;
  unsigned int y;
  unsigned int z;

  float x_voxel_size;
  float y_voxel_size;
  float z_voxel_size;

  int data_start;
  int data_end;
  bool raw;
};

/// Load NRRD files. Limited to following:
/// Only little endian
/// Only GZIP or raw encoded
/// Only 3D Data
/// Limitations are a result of usage in project.
class nrrd_loader {
 public:
  /// Loads NRRD file into memory
  ///@return a volume_block object containing the data from path
  ///@param path the path to load NRRD file from
  volume_block load_file(const std::string path);

 private:
  /// Loads the header of NRRD file
  ///@param path the path from which the NRRD file is to be loaded
  ///@return a NRRD header struct
  nrrd_header load_header(std::string path);

  /// Used for tokenising input
  void tokenise(std::string const &s, const char delimeter,
                std::vector<std::string> &out);
  /// Inflates GZIP encoded data
  int inflate_gzip(const void *source, int source_length, void *destination,
                   int destination_length);
};
