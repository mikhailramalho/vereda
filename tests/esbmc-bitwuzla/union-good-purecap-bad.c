#include <stdlib.h>
#include <stdio.h>

#define SIZE 5

void main(void)
{
  union
  {
    int *p0;
    struct
    {
      char p1[SIZE];
      int p2;
    } str;
  } data;
  data.p0 = (int *)malloc(sizeof(int *));
  data.str.p2 = 1;
  free(data.p0);
}
// Imported from ESBMC regression/bitwuzla/union-good-purecap-bad
// RUN: --solver bitwuzla
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
