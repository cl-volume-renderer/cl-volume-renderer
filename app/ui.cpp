#include "ui.h"
#include <SDL.h>
#include <string>
#include <SDL_opengl.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl2.h>

//including the implementation of the opengl implementation
#include <imgui_impl_opengl2.cpp>
#include <imgui_impl_sdl.cpp>
#include "nrrd_loader.h"

static const GLubyte emptyData[] = {0, 0, 0, 250};

ui::ui(const char *raw_path) {
  GLuint texture[1];

  path = std::string(raw_path);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    return;
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  window = SDL_CreateWindow("GPGPU - Volume renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, window_flags);
  gl_context = SDL_GL_CreateContext(window);

  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  //create texture for opencl for now we are filling that here with a empty color, so we dont have gargabe later.
  glGenTextures(1, texture);
  frametexture = texture[0];
  frametexture_fill(2, 2, emptyData);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL2_Init();
}

ui::~ui() {
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void * ui::gl_context_get(void) {
  return gl_context;
}

const GLuint ui::frametexture_get(void) {
  return frametexture;
}

void ui::frametexture_fill(unsigned int width, unsigned int height, const void *data)
{
  glBindTexture(GL_TEXTURE_2D, frametexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

static void
keysymbol_handle(struct ui_state *state, std::string key)
{
  if        (key == "W") {
    state->position[0] += 0.1f;
  } else if (key == "S") {
    state->position[0] -= 0.1f;
  } else if (key == "A") {
    state->position[2] -= 0.1f;
  } else if (key == "D") {
    state->position[2] += 0.1f;
  } else if (key == "E") {
    state->position[1] += 0.1f;
  } else if (key == "Q") {
    state->position[1] -= 0.1f;
  } else if (key == "Up") {
    state->direction_look[1] += 0.1f;
  } else if (key == "Down") {
    //cam up
    state->direction_look[1] -= 0.1f;
  } else if (key == "Right") {
    //cam right
    state->direction_look[0] += 0.1f;
  } else if (key == "Left") {
    //cam left
    state->direction_look[0] -= 0.1f;
  } else {
    return;
  }
  state->cam_changed = true;
  state->direction_look[0] = fmod(state->direction_look[0], 2*M_PI);
  state->direction_look[1] = fmod(state->direction_look[1], 2*M_PI);
}

void ui::run(frame_emitter *emitter) {

    char file_path[1024];
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool done = false;
    ImGuiIO& io = ImGui::GetIO();
    struct ui_state state = {path, true, 0, 0, {0,0,0}, {0, 0}, true};
    SDL_GetWindowSize(window, &state.width, &state.height); //We have a fixed window, so that is enough
    snprintf(file_path, 1024, "%s", path.c_str());
    nrrd_loader loader;
    volume_block v = loader.load_file(path);
    emitter->image_set(&v);

    while (!done) {
        bool frame_changed;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (ImGui_ImplSDL2_ProcessEvent(&event)) {
              if (event.type == SDL_KEYDOWN)
                keysymbol_handle(&state, SDL_GetKeyName(event.key.keysym.sym));
            }
            if (event.type == SDL_QUIT)
              done = true;

        }
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        ImGui::Begin("Camera positions");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Position:");
        ImGui::Text(std::to_string(state.position[0]).c_str());
        ImGui::Text(std::to_string(state.position[1]).c_str());
        ImGui::Text(std::to_string(state.position[2]).c_str());
        ImGui::Text("Direction-look:");
        ImGui::Text(std::to_string(state.direction_look[0]).c_str());
        ImGui::Text(std::to_string(state.direction_look[1]).c_str());
        //ImGui::InputText("File", file_path, 1024); FIXME - not yet
        ImGui::End();

        if (!!strcmp(file_path, path.c_str()))
          {
             printf("Changed: %s\n", file_path);
             path = std::string(file_path);
             state.path_changed = true;
          }

        ImGui::Render();

        //clear
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        void *framedata = emitter->render_frame(state, frame_changed);
        if (frame_changed)
          frametexture_fill(state.width, state.height, framedata);

        //draw the image rendered in opencl
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, frametexture);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-1, -1);
        glTexCoord2f(0, 1); glVertex2f(-1, 1);
        glTexCoord2f(1, 1); glVertex2f(1, 1);
        glTexCoord2f(1, 0); glVertex2f(1, -1);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        //draw imgui
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
}
