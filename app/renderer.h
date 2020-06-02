#pragma once
#include <ui.h>
#include <clw_context.h>
#include <clw_function.h>
#include <clw_image.h>
#include <string>

class renderer : public frame_emitter {
  private:
    clw_context ctx;
    clw_function render_func;
    clw_image<unsigned int> frame; //frame output in 2D
    clw_image<short> *reference_volume; //image data input in 3D
    clw_image<short> *buffer_volume; //image data input in 3D
  public:
    renderer();
    ~renderer();
    void image_set(volume_block *b) override;
    void* render_frame(struct ui_state &state, bool &frame_changed) override;
};
