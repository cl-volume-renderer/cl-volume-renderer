#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>
#include <string>
#include "volume_block.h"
#include <clw_image.h>
#include <clw_context.h>
#include "common.h"
#include "reference_volume.h"
#include "env_map.h"

struct ui_state{
   std::string path;
   bool path_changed;

   int height;
   int width;

   Position3D position; //0->x 1->y 3->z
   float direction_look[2]; //this are two angles that you can adjust

   bool cam_changed;
};

#include <tf_part.h>

class frame_emitter {
  public:
    virtual ~frame_emitter() { };
    virtual void image_set(const reference_volume *volume, const env_map *map) = 0;
    virtual void next_event_code_set(const std::string cl_code) = 0;
    virtual void flush_changes() = 0;
    virtual void* render_frame(struct ui_state &state, bool &frame_changed) = 0;
    virtual void* render_tf(const unsigned int width, const unsigned int height) = 0;
};

class ui {
  private:
    clw_context &ctx;
    SDL_Window *window;
    SDL_GLContext gl_context;
    GLuint frametexture;
    GLuint tftexture;
    std::string nrrd_path, env_map_path;
    void flush_tf(frame_emitter *emitter, Volume_Stats stats, std::vector<tf_selection*> selection);
  public:
    ui(const char *path, const char *env_map, clw_context &c);
    ~ui();
    void run(frame_emitter *emitter);
    const GLuint frametexture_get(void);
    void * gl_context_get(void);
    void frametexture_fill(unsigned int width, unsigned int height, const void *data);
};
