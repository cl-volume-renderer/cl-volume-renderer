#include "nrrd_loader.h"
#include <zlib.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// Keys for key/value parsing
constexpr const auto type_key = "type";
constexpr const auto size_key = "sizes";
constexpr const auto encoding_key = "encording";
constexpr const auto endian_key = "endian";
constexpr const auto dimension_key = "dimension";
constexpr const auto space_direction_key = "space directions";

nrrd_header nrrd_loader::load_header(std::string path) {
  nrrd_header header = {0};

  std::ifstream input_file;
  input_file.open(path, std::ios::in | std::ios::binary);
  if(input_file.fail()){
    std::cerr << "Error, failed to open .nrrd file: " << path << '\n';
    exit(1);
  }

  std::string line = "";

  while (std::getline(input_file, line)) {
    if (line.length() == 0) break;

    // Ignore version and comments
    if (line[0] == '#' || (line[0] == 'N' && line[1] == 'R' && line[2] == 'R' &&
                           line[3] == 'D')) {
      continue;
    }
    std::vector<std::string> out_tag;
    tokenise(line, ':', out_tag);
    std::string tag = out_tag[0];

    // Some error checking
    if (out_tag.size() < 2) {
      std::cerr << "Error: NRRD file does not declare tags correctly.\n";
      exit(EXIT_FAILURE);
    }

    std::vector<std::string> out;

    // trim spaces from left
    std::string value_string = out_tag[1].substr(
        out_tag[1].find_first_not_of(' '), out_tag[1].length());
    tokenise(value_string, ' ', out);

    if (tag == type_key) {
      if (out[0] != "short") {
        std::cerr << "Error: NRRD file not using short as type.\n";
        exit(EXIT_FAILURE);
      }
    } else if (tag == encoding_key) {
      if (out[0] == "gzip") {
        header.raw = false;
      } else if (out[0] == "raw") {
        header.raw = true;
      } else {
        std::cerr << "Error: NRRD file not using gzip compression or raw.\n";
        exit(EXIT_FAILURE);
      }
    } else if (tag == endian_key) {
      if (out[0] != "little") {
        std::cerr << "Error: NRRD file not using little endian format.\n";
        exit(EXIT_FAILURE);
      }
    } else if (tag == dimension_key) {
      if (out[0] != "3") {
        std::cerr << "Error: NRRD file not using dimension of 3.\n";
        exit(EXIT_FAILURE);
      }
    }
    if (tag == size_key) {
      if (out.size() != 3) {
        std::cerr << "Error: NRRD file does not declare sizes correctly.\n";
        exit(EXIT_FAILURE);
      }
      header.x = std::stoi(out[0]);
      header.y = std::stoi(out[1]);
      header.z = std::stoi(out[2]);
    }
    if (tag == space_direction_key) {
      if (out.size() != 3) {
        std::cerr
            << "Error: NRRD file does not declare space direction correctly.\n";
        exit(EXIT_FAILURE);
      }

      std::vector<std::string> entries;
      tokenise(out[0], ',', entries);
      header.x_voxel_size = std::stof(entries[0].substr(1));
      entries.clear();
      tokenise(out[1], ',', entries);
      header.y_voxel_size = std::stof(entries[1]);
      entries.clear();
      tokenise(out[2], ',', entries);
      header.z_voxel_size =
          std::stof(entries[2].substr(0, entries[2].length() - 1));

      header.z_voxel_size = header.z_voxel_size / header.x_voxel_size;
      header.y_voxel_size = header.y_voxel_size / header.x_voxel_size;
      header.x_voxel_size = header.x_voxel_size / header.x_voxel_size;  // = 1
    }
  }
  // stream now at data start
  int data_start = input_file.tellg();
  input_file.seekg(0, std::ios::end);
  int data_end = input_file.tellg();

  header.data_start = data_start;
  header.data_end = data_end;
  input_file.close();
  return header;
}
// block
// we need a map function, which maps the smaller volume into the voxel block
// Else, if all three dimensions are of different size, the standard mapping
// will not work
volume_block nrrd_loader::load_file(std::string path) {
  const nrrd_header header = load_header(path);
  const unsigned int inflated_size = header.x * header.y * header.z*4;

  std::vector<short> voxel_vector(header.x*header.y*header.z);

  std::ifstream input_file;
  input_file.open(path, std::ios_base::in | std::ios_base::binary);
  input_file.seekg(header.data_start);

  // We assume that data points to large enough memory chunk
  if (!header.raw) {
    std::vector<char> read_data(header.data_end - header.data_start);
    input_file.read(&read_data[0], header.data_end - header.data_start);
    inflate_gzip(&read_data[0], read_data.size(), voxel_vector.data(),
                 inflated_size);
  } else {
    input_file.read(reinterpret_cast<char *>(voxel_vector.data()),
                    header.data_end - header.data_start);
  }
  input_file.close();

  volume_block return_block(std::move(voxel_vector),header.x, header.y, header.z, header.x_voxel_size,
                            header.y_voxel_size, header.z_voxel_size);
  return return_block;
}

// Used for tokenising input
void nrrd_loader::tokenise(std::string const &s, const char delimeter,
                           std::vector<std::string> &out) {
  std::stringstream ss(s);

  std::string tmp_string;
  while (std::getline(ss, tmp_string, delimeter)) {
    out.push_back(tmp_string);
  }
}
// Inflates GZIP encoded data
int nrrd_loader::inflate_gzip(const void *source, int source_length,
                              void *destination, int destinationLen) {
  z_stream stream = {0};
  stream.total_in = stream.avail_in = source_length;
  stream.total_out = stream.avail_out = destinationLen;
  // Evil C-Style casts, but zlib is a C library
  stream.next_in = (Bytef *)source;
  stream.next_out = (Bytef *)destination;

  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;

  int err = -1;
  int ret = -1;

  err = inflateInit2(&stream, (15 + 32));  // 15 window bits, and the +32 tells
                                           // zlib to to detect if using gzip or
                                           // zlib
  if (err == Z_OK) {
    err = inflate(&stream, Z_FINISH);
    if (err == Z_STREAM_END) {
      ret = stream.total_out;
    } else {
      inflateEnd(&stream);
      return err;
    }
  } else {
    inflateEnd(&stream);
    return err;
  }

  inflateEnd(&stream);
  return ret;
}
