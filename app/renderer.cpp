#include "renderer.h"
#include "nrrd_loader.h"
#include "debug_helper.h"

std::vector<unsigned char> output = std::vector<unsigned char>(2048*1024*4);
std::vector<char> buf_init(8*2);
std::vector<short> ref_init(8);
std::vector<short> sdf_init(8);

renderer::renderer()
: render_func(ctx, "ray_marching.cl", "render"),
  frame(ctx, std::move(output), {2048, 1024,1}),
  reference_volume(ctx, std::move(ref_init), {2,2,2}),
  sdf(ctx, std::move(sdf_init), {2, 2, 2}),
  buffer_volume(ctx, std::move(buf_init))
{

}

renderer::~renderer()
{

}

static unsigned long
evenness(const unsigned int g, const unsigned int l)
{
   unsigned int mod = g % l;
   if (g % l == 0) return g;

   return g + 8 - mod;
}

void renderer::image_set(volume_block *b)
{
  //resources for rendering
  std::vector<char> buffer(b->m_voxel_count_x * b->m_voxel_count_y * b->m_voxel_count_z*2, 0);
  buffer_volume = clw_vector<char>(ctx, std::move(buffer));
  buffer_volume.push();
  reference_volume = clw_image<const short>(ctx, std::move(b->m_voxels), {b->m_voxel_count_x, b->m_voxel_count_y, b->m_voxel_count_z});
  reference_volume.push();

  //resources for the sdf caclulation
  std::vector<short> sdf_volume_buffer(b->m_voxel_count_x * b->m_voxel_count_y * b->m_voxel_count_z, 0);
  sdf = clw_image<short>(ctx, std::move(sdf_volume_buffer), {b->m_voxel_count_x, b->m_voxel_count_y, b->m_voxel_count_z});
  sdf.push();

  clw_image<short> sdf_volume_pong = clw_image<short>(ctx, std::move(sdf_volume_buffer), {b->m_voxel_count_x, b->m_voxel_count_y, b->m_voxel_count_z});
  sdf_volume_pong.push();

  //first fill the sdf image with -1 for *inside* a interesting area 1 outside a interesting area
  auto sdf_generation_init = clw_function(ctx, "signed_distance_field.cl", "create_boolean_image");
  sdf_generation_init.execute({evenness(b->m_voxel_count_x, 8), evenness(b->m_voxel_count_y,8), evenness(b->m_voxel_count_z, 8)}, {4,4,4}, reference_volume, sdf);

  //now iterate
  std::vector<int> counter(1,0);
  clw_vector<int> atomic_add_buffer(ctx, std::move(counter));
  auto sdf_generation_initialization = clw_function(ctx, "signed_distance_field.cl", "create_signed_distance_field");
  printf("Starting to build SDF\n");
  clock_t start = clock();
  for (int i = 0; i < 1000; ++i) {
    atomic_add_buffer[0] = 0;
    atomic_add_buffer.push();
    //FIXME this is only needed because swap does not like clw_image
    if (i % 2 == 0)
      sdf_generation_initialization.execute({evenness(b->m_voxel_count_x, 8), evenness(b->m_voxel_count_y,8), evenness(b->m_voxel_count_z, 8)}, {4,4,4}, sdf, sdf_volume_pong, atomic_add_buffer);
    else
      sdf_generation_initialization.execute({evenness(b->m_voxel_count_x, 8), evenness(b->m_voxel_count_y,8), evenness(b->m_voxel_count_z, 8)}, {4,4,4}, sdf_volume_pong, sdf, atomic_add_buffer);
    atomic_add_buffer.pull();
    if (atomic_add_buffer[0] == 0 && i % 2 == 1) { //FIXME the later part is only required because sdf is not always filled with the correct values
      break;
    }
  }
  printf("Finished building SDF (%f)\n", (float)(clock() - start) / CLOCKS_PER_SEC);

  //this here is needed for nvidia to ensure that in another call with sdf as parameter to not contain gargabe
  sdf.pull();
  sdf.push();
}


static Position3D
rotate_vector(float alpha, float beta, float gamma, Position3D vec)
{
  Position3D direction_vector = {
    (cos(alpha)*cos(beta))*vec.val[0] +            (cos(alpha)*sin(beta)-sin(alpha)*cos(gamma))*vec.val[1] + (cos(alpha)*sin(beta)*cos(gamma)+sin(alpha)*sin(gamma))*vec.val[2],
              (-sin(beta))*vec.val[0] +                                  (cos(beta)*sin(gamma))*vec.val[1] +                                  (cos(beta)*cos(gamma))*vec.val[2],
    (sin(alpha)*cos(beta))*vec.val[0] + (sin(alpha)*sin(beta)*sin(gamma)+cos(alpha)*cos(gamma))*vec.val[1] + (sin(alpha)*sin(beta)*cos(gamma)-cos(alpha)*sin(gamma))*vec.val[2],
  };

  float length = direction_vector.length();
  return direction_vector / length;
}


void* renderer::render_frame(struct ui_state &state, bool &frame_changed)
{
  frame_changed = false;
  if (!state.cam_changed && !state.path_changed)
    return &frame[0];

  //for now we dont have dynamic frame sizes, so we need to have some assertion that the frame size is not magically changing.
  //assert(frame.size() == state.width * state.height * 4);

  Position3D vec = rotate_vector(state.direction_look[0], state.direction_look[1], 0.0, {1.0, 0.0, 0.0});

  TIME_START();
  render_func.execute({(unsigned long)state.width, (unsigned long)state.height}, {8, 8}, frame,
    reference_volume, sdf, buffer_volume,
    state.position.val[0], state.position.val[1], state.position.val[2],
    vec.val[0], vec.val[1], vec.val[2]);

  frame.pull();
  TIME_PRINT("Render time");

  state.cam_changed = false;
  state.path_changed = false;
  frame_changed = true;

  return &frame[0];
}

