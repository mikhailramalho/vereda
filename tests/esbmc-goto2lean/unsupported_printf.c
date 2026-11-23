#include <stdio.h>
int main(void) {
  printf("%d\n", *(int *)0);
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_printf
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported statement code_printf
// Vereda: verdict none
