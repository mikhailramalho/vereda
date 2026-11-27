#include <stdio.h>

int main(int argc, char *argv[])
{
  char *toParseStr = (char *)malloc(11);
  printf("Enter string here: ");
  scanf("%10s", toParseStr);
  printf("%s", toParseStr);
  free(toParseStr);
}
// Imported from ESBMC regression/cstd/scanf_malloc_2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
