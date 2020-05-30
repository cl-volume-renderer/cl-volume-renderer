#include <iostream>
#include <clw_context.h>
#include "ui.h"

int main(){
  Ui ui = Ui();
  clw_context ctx = clw_context(ui.gl_context_get());
  ui.run();
  return 0;
}
