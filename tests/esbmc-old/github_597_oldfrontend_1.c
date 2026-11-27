#include <stdio.h>
int main() {
  printf("Hello, World!");
  return 0;
}
// Imported from ESBMC regression/esbmc-old/github_597_oldfrontend_1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
