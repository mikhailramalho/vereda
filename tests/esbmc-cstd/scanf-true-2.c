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
// Imported from ESBMC regression/cstd/scanf-true-2
// Needs: --no-unlimited-scanf-check
// RUN: --overflow-check --64 --no-unlimited-scanf-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
