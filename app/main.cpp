#include <iostream>
#include <clw_context.h>
#include <clw_foreign_memory.h>
#include <clw_function.h>
#include "ui.h"
#include "renderer.h"
#include "clw_context.h"

int main(int argc, char *argv[]){
  clw_context ctx;
  renderer render_ctx = renderer(ctx);
  ui ui_ctx = ui(argc > 1 ? argv[1] : "", ctx);
  ui_ctx.run(&render_ctx);
  return 0;
}
