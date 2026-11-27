#include <stdlib.h>
#include <assert.h>
int main(){
  size_t len = 2;
  char *p = malloc(len - 4);
  if (p) p[0] = 1;
  assert(0);
}
// Imported from ESBMC regression/esbmc/default_underflow_malloc
// RUN: --no-incremental-bmc
// EXPECT: ^Violated property:$
// EXPECT: ^VERIFICATION FAILED$
