#include "renderer.h"
#include "nrrd_loader.h"

std::vector<unsigned char> output = std::vector<unsigned char>(1024*512*4);
std::vector<short> empty = std::vector<short>(1024*512*4);

renderer::renderer()
: render_func(ctx, "ray_marching.cl", "render"),
  frame(ctx, std::move(output), {1024, 512,1})
{

}

renderer::~renderer()
{

}

void renderer::image_set(volume_block *b)
{
  if (reference_volume == NULL)
    delete reference_volume;

  if (buffer_volume == NULL)
    delete buffer_volume;

  buffer_volume = new clw_image<short>(ctx, std::move(b->m_voxels), {b->m_voxel_count_x, b->m_voxel_count_y, b->m_voxel_count_z});
  reference_volume = new clw_image<short>(ctx, std::move(b->m_voxels), {b->m_voxel_count_x, b->m_voxel_count_y, b->m_voxel_count_z});
}

void* renderer::render_frame(struct ui_state &state, bool &frame_changed)
{
  frame_changed = false;
  if (!state.cam_changed && !state.path_changed)
    return &frame[0];

  //for now we dont have dynamic frame sizes, so we need to have some assertion that the frame size is not magically changing.
  //assert(frame.size() == state.width * state.height * 4);

  //render_func.execute({(unsigned long)state.width, (unsigned long)state.height}, {8, 8}, frame, state.width, state.height);
  render_func.execute({(unsigned long)state.width, (unsigned long)state.height}, {8, 8}, frame, *reference_volume, *buffer_volume, state.direction_look[0], state.direction_look[1], state.direction_look[2], state.position[0], state.position[1], state.position[2]);
  frame.pull();

  state.cam_changed = false;
  state.path_changed = false;
  frame_changed = true;

  return &frame[0];
}

