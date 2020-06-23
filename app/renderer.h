#pragma once
#include <ui.h>
#include <clw_context.h>
#include <clw_function.h>
#include <clw_image.h>
#include <string>
#include "reference_volume.h"

class renderer : public frame_emitter {
  private:
    clw_context &ctx;
    clw_function render_func;
    clw_image<unsigned char,4> frame; //frame output in 2D
    clw_image<short> sdf;
    clw_vector<short> buffer_volume; //image data input in 3D
    clw_image<unsigned char, 4> tfframe;
    reference_volume *volume; //image data input in 3D
    std::string local_cl_code;
    void calc_sdf();
  public:
    renderer(clw_context &ctx);
    ~renderer();
    void image_set(reference_volume *rv) override;
    void* render_frame(struct ui_state &state, bool &frame_changed) override;
    void* render_tf(const unsigned int width, const unsigned int height) override;
    void next_event_code_set(const std::string cl_code) override;
};
