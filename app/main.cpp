#include <iostream>
#include <clw_context.h>
#include <clw_foreign_memory.h>
#include <clw_function.h>
#include "ui.h"
#include "renderer.h"

int main(int argc, char *argv[]){
  renderer render_ctx = renderer();
  ui ui_ctx = ui(argc > 1 ? argv[1] : "");
  ui_ctx.run(&render_ctx);
  return 0;
}
