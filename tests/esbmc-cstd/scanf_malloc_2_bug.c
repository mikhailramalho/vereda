#include <stdio.h>

int main(int argc, char *argv[])
{
  char *toParseStr = (char *)malloc(11);
  printf("Enter string here: ");
  scanf("%12s", toParseStr);
  printf("%s", toParseStr);
  free(toParseStr);
}
// Imported from ESBMC regression/cstd/scanf_malloc_2_bug
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
