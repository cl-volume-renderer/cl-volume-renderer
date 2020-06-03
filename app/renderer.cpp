#include "renderer.h"
#include "nrrd_loader.h"

std::vector<unsigned char> output = std::vector<unsigned char>(2048*1024*4);

renderer::renderer()
: render_func(ctx, "ray_marching.cl", "render"),
  frame(ctx, std::move(output), {2048, 1024,1})
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

  std::vector<short> buffer(b->m_voxel_count_x * b->m_voxel_count_y * b->m_voxel_count_z, -4000);
  buffer_volume = new clw_image<short>(ctx, std::move(buffer), {b->m_voxel_count_x, b->m_voxel_count_y, b->m_voxel_count_z});
  buffer_volume->push();
  reference_volume = new clw_image<const short>(ctx, std::move(b->m_voxels), {b->m_voxel_count_x, b->m_voxel_count_y, b->m_voxel_count_z});
  reference_volume->push();
}

void* renderer::render_frame(struct ui_state &state, bool &frame_changed)
{
  frame_changed = false;
  if (!state.cam_changed && !state.path_changed)
    return &frame[0];

  //for now we dont have dynamic frame sizes, so we need to have some assertion that the frame size is not magically changing.
  //assert(frame.size() == state.width * state.height * 4);

  float direction_vector[3] = {
                cos(state.direction_look[0]) * cos(state.direction_look[1]),
                sin(state.direction_look[1]),
                sin(state.direction_look[0]) * cos(state.direction_look[1])
              };

  float length = sqrtf(pow(direction_vector[0], 2) + pow(direction_vector[1], 2) + pow(direction_vector[2], 2));
  direction_vector[0] /= length;
  direction_vector[1] /= length;
  direction_vector[2] /= length;

  render_func.execute({(unsigned long)state.width, (unsigned long)state.height}, {8, 8}, frame,
    *reference_volume, *buffer_volume,
    state.position[0], state.position[1], state.position[2],
    direction_vector[0], direction_vector[1], direction_vector[2]);
  frame.pull();

  state.cam_changed = false;
  state.path_changed = false;
  frame_changed = true;

  return &frame[0];
}

