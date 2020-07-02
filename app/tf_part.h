#ifndef TF_PART_H
#define TF_PART_H 1

#include <array>
#include <string>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>
#include "reference_volume.h"

class tf_selection {
  public:
    virtual ~tf_selection() { };
    virtual void render_ui(ImVec2 position, ImVec2 size, Volume_Stats stats) = 0;
    virtual std::string create_cl_condition(Volume_Stats stats) = 0;
};

class tf_rect_selection : public tf_selection {
  private:
    unsigned int id;
    float min_v, max_v, min_g, max_g;
    float color[4];
  public:
    tf_rect_selection(unsigned int id, float min_v, float max_v, float min_g, float max_g);
    virtual ~tf_rect_selection() { };
    void render_ui(ImVec2 position, ImVec2 size, Volume_Stats stats) override;
    std::string create_cl_condition(Volume_Stats stats) override;
};

class tf_circle_segment_selection : public tf_selection {
  private:
    unsigned int id;
    float point_v, point_g;
    float radius;
    float degree;
    float color[4];
  public:
    tf_circle_segment_selection( unsigned int id, float point_v, float point_g, float radius, float degree);
    virtual ~tf_circle_segment_selection() { };
    void render_ui(ImVec2 position, ImVec2 size, Volume_Stats stats) override;
    std::string create_cl_condition(Volume_Stats stats) override;
};


#endif
