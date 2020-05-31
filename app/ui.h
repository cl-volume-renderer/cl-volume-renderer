#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>

class ui {
  private:
  SDL_Window *window;
  SDL_GLContext gl_context;
  GLuint frametexture;
  public:
    ui();
    ~ui();
    void run(void);
    const GLuint frametexture_get(void);
    void * gl_context_get(void);
    void frametexture_fill(unsigned int width, unsigned int height, const void *data);
};
