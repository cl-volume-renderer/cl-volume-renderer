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

void tf_rect_selection::render_ui(ImVec2 position, ImVec2 size, Volume_Stats stats) {
   ImGui::PushID(id);
   if (ImGui::CollapsingHeader("Rect:", id)) {
     ImGui::SliderFloat("Min Value", &min_v, stats.min_v, stats.max_v);
     ImGui::SliderFloat("Max Value", &max_v, stats.min_v, stats.max_v);
     ImGui::SliderFloat("Min Gradient", &min_g, stats.min_g, stats.max_g);
     ImGui::SliderFloat("Max Gradient", &max_g, stats.min_g, stats.max_g);
     ImGui::ColorEdit4("Color", color);
   }
   ImGui::PopID();

   min_v = std::min(min_v, max_v);
   max_v = std::max(max_v, min_v);
   min_g = std::min(min_g, max_g);
   max_g = std::max(max_g, min_g);

   float v_distance = (stats.max_v - stats.min_v);
   float g_distance = (stats.max_g - stats.min_g);
   float tmp_min_v = (min_v - stats.min_v)/v_distance;
   float tmp_max_v = (max_v - stats.min_v)/v_distance;
   float tmp_min_g = (min_g - stats.min_g)/g_distance;
   float tmp_max_g = (max_g - stats.min_g)/g_distance;

   ImVec2 rposition = {tmp_min_v*size.x, tmp_min_g*size.y};
   ImVec2 rsize = {(tmp_max_v - tmp_min_v)*size.x, (tmp_max_g - tmp_min_g)*size.y};

   position.x += rposition.x;
   position.y += (size.y - rposition.y) - rsize.y;

   rsize.x += position.x;
   rsize.y += position.y;

   ImDrawList *dlist = ImGui::GetWindowDrawList();
   dlist->AddRect(position, rsize, ImColor(color[0], color[1], color[2]));
   dlist->AddRectFilled(position, rsize, ImColor(color[0], color[1], color[2], 0.2f));
}

std::string tf_rect_selection::create_cl_condition(Volume_Stats stats) {
  /*
   * {min,max}{_v, _g} are between 0.0 and 1.0
   */
  std::ostringstream cl_code;
  cl_code << "  if(value >= ";
  cl_code << min_v;
  cl_code << " && value <= ";
  cl_code << max_v;
  /*cl_code << " && gradient > ";
  cl_code << stats.min_g + (min_g)*(stats.max_g - stats.min_g);
  cl_code << " && gradient < ";
  cl_code << stats.min_g + (max_g)*(stats.max_g - stats.min_g);*/
  cl_code << ")\n";
  cl_code << " {\n";
  cl_code << "    int4 tmp_color = {" << color[0] << "," << color[1]  << "," << color[2] << "," << color[3] << "};\n";
  cl_code << "    *color = tmp_color;\n";
  cl_code << "    return true;\n";
  cl_code << " }\n";
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

void tf_circle_segment_selection::render_ui(ImVec2 position, ImVec2 size, Volume_Stats stats) {
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

std::string tf_circle_segment_selection::create_cl_condition(Volume_Stats stats) {
   return "";
}
