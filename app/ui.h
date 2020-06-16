#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>
#include <string>
#include "volume_block.h"
#include <clw_image.h>
#include <clw_context.h>

struct Position3D {
   float val[3];
   Position3D operator+(Position3D other) {
    Position3D ret = {
      val[0] + other.val[0],
      val[1] + other.val[1],
      val[2] + other.val[2]
    };
    return ret;
   }
   Position3D operator-(Position3D other) {
    Position3D ret = {
      val[0] - other.val[0],
      val[1] - other.val[1],
      val[2] - other.val[2]
    };
    return ret;
   }
   Position3D operator*(float other) {
    Position3D ret = {
      val[0] * other,
      val[1] * other,
      val[2] * other
    };
    return ret;
   }
   Position3D operator/(float other) {
    Position3D ret = {
      val[0] / other,
      val[1] / other,
      val[2] / other
    };
    return ret;
   }
   double length() {
    return sqrtf(pow(val[0], 2) + pow(val[1], 2) + pow(val[2], 2));
   }
};

struct ui_state{
   std::string path;
   bool path_changed;

   int height;
   int width;

   Position3D position; //0->x 1->y 3->z
   float direction_look[2]; //this are two angles that you can adjust

   bool cam_changed;
};

class frame_emitter {
  public:
    clw_context ctx;
    virtual ~frame_emitter() { };
    virtual void image_set(volume_block *b) = 0;
    virtual void* render_frame(struct ui_state &state, bool &frame_changed) = 0;
};

class ui {
  private:
  SDL_Window *window;
  SDL_GLContext gl_context;
  GLuint frametexture;
  std::string path;
  public:
    ui(const char *path);
    ~ui();
    void run(frame_emitter *emitter);
    const GLuint frametexture_get(void);
    void * gl_context_get(void);
    void frametexture_fill(unsigned int width, unsigned int height, const void *data);
};
