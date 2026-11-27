#include <stdio.h>
#include <stdint.h>
int main(int argc, char *argv[])
{
  int x;
  long long y;
  char z[10];

  scanf("%d%ld%s", &x, &y, z);

  return 0;
}
// Imported from ESBMC regression/cstd/scanf-false
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
