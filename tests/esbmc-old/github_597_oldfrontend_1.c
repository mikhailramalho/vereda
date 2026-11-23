#include <stdio.h>
int main() {
  printf("Hello, World!");
  return 0;
}
// Imported from ESBMC regression/esbmc-old/github_597_oldfrontend_1
// Needs: --old-frontend
// RUN: --old-frontend
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
