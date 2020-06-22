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
    clw_image<const short> reference_volume; //image data input in 3D
    clw_image<short> sdf;
    clw_vector<short> buffer_volume; //image data input in 3D
    clw_image<unsigned char, 4> tfframe;
    std::array<size_t, 3> volume_size;
    Histogram_Stats hs;
    std::string local_cl_code;
    void calc_sdf();
  public:
    renderer();
    ~renderer();
    void image_set(volume_block *b) override;
    void* render_frame(struct ui_state &state, bool &frame_changed) override;
    void* render_tf(const unsigned int width, const unsigned int height) override;
    void next_event_code_set(const std::string cl_code) override;
    Histogram_Stats fetch_histogram_stats() override;
};
