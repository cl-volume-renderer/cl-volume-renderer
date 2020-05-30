#include <iostream>
#include <clw_context.h>
#include "ui.h"

int main(){
  ui ui_ctx = ui();
  clw_context ctx = clw_context(ui_ctx.gl_context_get());
  ui_ctx.run();
  return 0;
}
