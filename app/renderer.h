#pragma once
#include <ui.h>
#include <clw_context.h>
#include <clw_function.h>

class renderer : public frame_emitter {
  private:
    clw_context ctx;
    clw_function render_func;
    clw_vector<unsigned char> frame;
  public:
    renderer();
    ~renderer();
    void* render_frame(struct ui_state &state, bool &frame_changed) override;
};
