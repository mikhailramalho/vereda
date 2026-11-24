#include <stdlib.h>
#include <stdio.h>

int main()
{
  int *i;
  i = (int *)malloc(sizeof(*i));
  printf("i=%p\n", i);
  free(i);
  printf("i=%p\n", i);
  free(i);
  printf("i=%p\n", i);

  return 0;
}
// Imported from ESBMC regression/bitwuzla/double-free
// RUN: --solver bitwuzla
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
