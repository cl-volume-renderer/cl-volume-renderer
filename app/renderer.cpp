#include "renderer.h"

std::vector<unsigned char> output = std::vector<unsigned char >(1024*512*4); //FIXME that is clunky

renderer::renderer()
: render_func(ctx, "hello_world.cl", "render_pixels"),
  frame(ctx, std::move(output))
{

}

renderer::~renderer()
{

}

void* renderer::render_frame(struct ui_state &state, bool &frame_changed)
{
  //FIXME we want to have = operator
  //std::vector<unsigned char> new_frame(state.width*state.height*4);
  //frame = clw_vector<unsigned char>(ctx, std::move(new_frame));
  frame_changed = false;
  if (!state.changed)
    return &frame[0];
  state.changed = false;
  frame_changed = true;

  render_func.run<1024, 512, 8, 8>(frame, state.width, state.height); //FIXME that is clunky
  frame.pull();

  return &frame[0];
}

