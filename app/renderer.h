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
    clw_image<unsigned char,4> frame; //frame output in 2D
    clw_image<const short> *reference_volume; //image data input in 3D
    clw_image<char, 2> *buffer_volume; //image data input in 3D
  public:
    renderer();
    ~renderer();
    void image_set(volume_block *b) override;
    void* render_frame(struct ui_state &state, bool &frame_changed) override;
};
