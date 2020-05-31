#include <iostream>
#include <clw_context.h>
#include <clw_foreign_memory.h>
#include <clw_function.h>
#include "ui.h"

int main(){
  std::vector<unsigned char> some_vals{250, 0, 0, 250, 250, 0, 0, 250, 250, 0, 0, 250, 250, 0, 0, 250};

  std::vector<unsigned char> output = std::vector<unsigned char >(128*128*4);
  ui ui_ctx = ui();
  clw_context ctx = clw_context(ui_ctx.gl_context_get());
  clw_vector<unsigned char> my_vec(ctx, std::move(output));
  clw_function my_func(ctx, "hello_world.cl", "render_pixels");
  my_func.run<128, 128, 8, 8>(my_vec, 128, 128);
  my_vec.pull();
  ui_ctx.frametexture_fill(128, 128, &my_vec[0]);
  ui_ctx.run();
  return 0;
}
