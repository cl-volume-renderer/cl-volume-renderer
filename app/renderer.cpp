#include "renderer.hpp"
#include "nrrd_loader.hpp"
#include "debug_helper.hpp"
#include <cstdlib>
#include <set>
#include "common_defines.hpp"

renderer::renderer(clw_context &c)
: ctx(c),
  render_func(ctx, "empty.cl", "empty"),
  frame(ctx, std::vector<unsigned char>(SCREEN_WIDTH*SCREEN_HEIGHT*4), {SCREEN_WIDTH, SCREEN_HEIGHT,1}),
  buffer_volume(ctx, std::vector<unsigned short>(8*4)),
  tfframe(ctx,  std::vector<unsigned char>(2*2*4), {2, 2}),
  sdf(ctx)
{

}

void renderer::image_set(const reference_volume *rv, const env_map *map)
{
  volume = rv;
  emap = map;
}

void renderer::flush_changes()
{
  TIME_START()
  //resources for rendering
  if (buffer_volume.size() != volume->get_volume_length()*4)// {
    buffer_volume = clw_vector<unsigned short>(ctx, std::vector<unsigned short>(volume->get_volume_length()*4), false);

  auto buffer_reset = clw_function(ctx, "buffer_reset.cl", "buffer_reset");
  buffer_reset.execute(
    volume->get_volume_size_evenness(4),
    {4,4,4}, volume->get_reference_volume(), buffer_volume);
  TIME_PRINT("buffer creation time");

  //flush changes to render func
  render_func = clw_function(ctx, "ray_marching.cl", "render", local_cl_code);

  //rebuild sdf with correct is_event func.
  sdf = signed_distance_field(ctx, *volume, local_cl_code);
}

void* renderer::render_tf(const unsigned int height, const unsigned int width)
{
  TIME_START();
  std::vector<unsigned char> tf_buffer(height*width*4);
  clw_image<unsigned char, 4> tmp(ctx, std::move(tf_buffer), {width, height});
  tfframe = std::move(tmp);

  //then we are counting
  std::vector<unsigned int> frame_buffer(width*height, 0);
  clw_vector<unsigned int> frame(ctx, std::move(frame_buffer));
  frame.push();

  auto tf_frame_construction = clw_function(ctx, "histogram.cl", "tf_sort_values");
  tf_frame_construction.execute(
    volume->get_volume_size_evenness(8),
    {4,4,4}, volume->get_reference_volume(), frame, width, height,
    volume->get_volume_stats().min_v, volume->get_volume_stats().max_v, volume->get_volume_stats().min_g, volume->get_volume_stats().max_g);
  //build a map from the value to the position of ranking all values in the frame
  frame.pull();

  std::set<int, std::less<int>> possible_histories;
  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      int value = frame[x*height+y];
      if (value != 0) {
        //we round a number onto the next lower 10'th position
        //what we could observe in most datasets is that higher counts are more likely than lower,
        //but lower counts are producing more details, so we correct in this way
        int roundingpart = std::max((int)(pow(10, (std::floor(std::log10(value))) - 1)), (int)1);
        int corrected_value = floor(value/roundingpart)*roundingpart;
        frame[x*height+y] = corrected_value;
        possible_histories.insert(corrected_value);
      }
    }
  }
  frame.push();


  //TODO Why does this happen on Intel HD Graphics for large volumes?
  if(possible_histories.size() == 0){
    std::cout << "Warning, histogram does not contain non-zero entries.\n";
  }else{
    std::vector<int> history_buffer;
    std::copy(possible_histories.begin(), possible_histories.end(), std::back_inserter(history_buffer));
    clw_vector<int> history(ctx, std::move(history_buffer));
    history.push();

    auto tf_frame_flush = clw_function(ctx, "histogram.cl", "tf_flush_color_frame");
    tf_frame_flush.execute(
      {evenness(tfframe.get_dimensions()[0], 16), evenness(tfframe.get_dimensions()[1], 16)},
      {16,16}, tfframe, frame, history, (int)history.size());

  }
  TIME_PRINT("Tf render time");

  /*
  frame.pull();
  stats.pull();
  tfframe.pull();
  for (int x = 0; x < 50; ++x) {
    for (int y = 0; y < 50; ++y) {
      printf("%d, ", frame[(y*50+x)]);
    }
    printf("\n");
  }

  for (int i = 0; i < 5; ++i) {
    printf("%d\n", stats[i]);
  }

  for (int x = 0; x < 50; ++x) {
    for (int y = 0; y < 50; ++y) {
      printf("%d, ", tfframe[(y*50+x)*4]);
    }
    printf("\n");
  }
*/
  tfframe.pull();
  return &tfframe[0];
}

void renderer::next_event_code_set(const std::string cl_code)
{
   local_cl_code = cl_code;
}

void* renderer::render_frame(struct ui_state &state, bool &frame_changed)
{
  frame_changed = false;
  if (!state.cam_changed && !state.path_changed)
    return &frame[0];

  //for now we dont have dynamic frame sizes, so we need to have some assertion that the frame size is not magically changing.
  //assert(frame.size() == state.width * state.height * 4);

  Position3D vec(state.direction_look[0], state.direction_look[1], 0.0, {1.0, 0.0, 0.0});

  int random_seed = std::rand();

  TIME_START();
  render_func.execute({(unsigned long)state.width, (unsigned long)state.height}, {8, 8}, frame,
    volume->get_reference_volume(), sdf.get_sdf_buffer(), emap->get_buffer(), buffer_volume,
    state.position.val[0], state.position.val[1], state.position.val[2],
    vec.val[0], vec.val[1], vec.val[2], random_seed);

  frame.pull();
  TIME_PRINT("Render time");

  state.cam_changed = false;
  state.path_changed = false;
  frame_changed = true;

  return &frame[0];
}

