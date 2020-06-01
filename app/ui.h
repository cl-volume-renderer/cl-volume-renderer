#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>

struct ui_state{
   int height;
   int width;

   float position[3];
   float direction_look[2];

   bool changed;
};

class frame_emitter {
  public:
    virtual ~frame_emitter() { };
    virtual void* render_frame(struct ui_state &state, bool &frame_changed) = 0;
};

class ui {
  private:
  SDL_Window *window;
  SDL_GLContext gl_context;
  GLuint frametexture;
  public:
    ui();
    ~ui();
    void run(frame_emitter *emitter);
    const GLuint frametexture_get(void);
    void * gl_context_get(void);
    void frametexture_fill(unsigned int width, unsigned int height, const void *data);
};
