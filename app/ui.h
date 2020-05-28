#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>

class Ui {
  private:
  SDL_Window *window;
  SDL_GLContext gl_context;
  public:
    Ui();
    ~Ui();
    void run(void);
};
