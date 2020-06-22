#include <sstream>
#include <ui.h>
#include <tf_part.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>
#include <math.h>

tf_rect_selection::tf_rect_selection(uint id,
  float min_v, float max_v,
  float min_g, float max_g) : id(id*100), min_v(min_v), max_v(max_v), min_g(min_g), max_g(max_g)
{
   color[0] = 255;
   color[1] = 0;
   color[2] = 0;
   color[3] = 255;
}

void tf_rect_selection::render_ui(ImVec2 position, ImVec2 size) {
   ImGui::PushID(id);
   if (ImGui::CollapsingHeader("Rect:", id)) {
     ImGui::SliderFloat("Min Value", &min_v, 0.0, 1.0);
     ImGui::SliderFloat("Max Value", &max_v, 0.0, 1.0);
     ImGui::SliderFloat("Min Gradient", &min_g, 0.0, 1.0);
     ImGui::SliderFloat("Max Gradient", &max_g, 0.0, 1.0);
     ImGui::ColorEdit4("Color", color);
   }
   ImGui::PopID();

   min_v = std::min(min_v, max_v);
   max_v = std::max(max_v, min_v);
   min_g = std::min(min_g, max_g);
   max_g = std::max(max_g, min_g);

   ImVec2 rposition = {min_v*size.x, min_g*size.y};
   ImVec2 rsize = {(max_v - min_v)*size.x, (max_g - min_g)*size.y};

   position.x += rposition.x;
   position.y += (size.y - rposition.y) - rsize.y;

   rsize.x += position.x;
   rsize.y += position.y;

   ImDrawList *dlist = ImGui::GetWindowDrawList();
   dlist->AddRect(position, rsize, ImColor(color[0], color[1], color[2]));
   dlist->AddRectFilled(position, rsize, ImColor(color[0], color[1], color[2], 0.2f));
}

std::string tf_rect_selection::create_cl_condition(Histogram_Stats &stats) {
  /*
   * {min,max}{_v, _g} are between 0.0 and 1.0
   */
  std::ostringstream cl_code;
  cl_code << "  if(value > ";
  cl_code << stats.min_v + (min_v)*(stats.max_v - stats.min_v);
  cl_code << " && value < ";
  cl_code << stats.min_v + (max_v)*(stats.max_v - stats.min_v);
  cl_code << " && gradient > ";
  cl_code << stats.min_g + (min_g)*(stats.max_g - stats.min_g);
  cl_code << " && gradient < ";
  cl_code << stats.min_g + (max_g)*(stats.max_g - stats.min_g);
  cl_code << ")\n";
  cl_code << "    return true;";
  return cl_code.str();
}


tf_circle_segment_selection::tf_circle_segment_selection(uint id,
  float point_v, float point_g, float radius, float degree) : id(id),
    point_v(point_v), point_g(point_g), radius(radius), degree(degree)
{
   color[0] = 0;
   color[1] = 255;
   color[2] = 0;
   color[3] = 255;
}

void tf_circle_segment_selection::render_ui(ImVec2 position, ImVec2 size) {
  ImGui::PushID(id);
  if (ImGui::CollapsingHeader("Circle Segment(%d):", id)) {
    ImGui::SliderFloat("Point v", &point_v, 0.0, 1.0);
    ImGui::SliderFloat("Point g", &point_g, 0.0, 1.0);
    ImGui::SliderFloat("Radius", &radius, 0.0, 1.0);
    ImGui::SliderFloat("Degrees", &degree, 0.0, 2*M_PI);
    ImGui::ColorEdit4("Color", color);
  }
  ImGui::PopID();
  //FIXME Implement
}

std::string tf_circle_segment_selection::create_cl_condition(Histogram_Stats &stats) {
   return "";
}
