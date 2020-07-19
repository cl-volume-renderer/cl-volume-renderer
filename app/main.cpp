#include <iostream>
#include <clw_context.hpp>
#include <clw_foreign_memory.hpp>
#include <clw_function.hpp>
#include "ui.hpp"
#include "renderer.hpp"

int main(int argc, char *argv[]){
  if (argc != 3) {
    std::cout << "Error, required 2 parameters, nrrd & hdre.";
    return -1;
  }
  clw_context ctx;
  renderer render_ctx = renderer(ctx);
  ui ui_ctx = ui(argv[1], argv[2], ctx);
  ui_ctx.run(&render_ctx);
  return 0;
}
