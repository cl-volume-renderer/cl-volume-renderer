#include <iostream>
#include <clw_context.h>
#include <clw_foreign_memory.h>
#include <clw_function.h>
#include "ui.h"
#include "renderer.h"
#include "clw_context.h"

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
