#pragma once
#include "reference_volume.h"


class signed_distance_field {
  private:
    clw_image<short> sdf;
  public:
    signed_distance_field(clw_context &c);
    signed_distance_field(clw_context &c, const reference_volume &rf, std::string local_cl_code);
    clw_image<short>& get_sdf_buffer();
};
