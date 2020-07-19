#pragma once
#include "reference_volume.hpp"


class signed_distance_field {
  private:
    clw_image<char> sdf;
  public:
    signed_distance_field(clw_context &c);
    signed_distance_field(clw_context &c, const reference_volume &rf, std::string local_cl_code);
    clw_image<char>& get_sdf_buffer();
};
