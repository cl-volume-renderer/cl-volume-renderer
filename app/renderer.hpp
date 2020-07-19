#pragma once
#include <ui.hpp>
#include <clw_context.hpp>
#include <clw_function.hpp>
#include <clw_image.hpp>
#include <string>
#include "reference_volume.hpp"
#include "signed_distance_field.hpp"

class renderer : public frame_emitter {
  private:
    clw_context &ctx;
    clw_function render_func;
    clw_image<unsigned char,4> frame; //frame output in 2D
    clw_vector<unsigned short> buffer_volume; //image data input in 3D
    clw_image<unsigned char, 4> tfframe;
    const reference_volume *volume; //image data input in 3D
    const env_map *emap;
    signed_distance_field sdf;
    std::string local_cl_code;
  public:
    renderer(clw_context &ctx);
    void image_set(const reference_volume *rv, const env_map *map) override;
    void flush_changes() override;
    void* render_frame(struct ui_state &state, bool &frame_changed) override;
    void* render_tf(const unsigned int width, const unsigned int height) override;
    void next_event_code_set(const std::string cl_code) override;

};
