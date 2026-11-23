#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  char *s = (char *)malloc(100);
  strncpy(s, "runoob", 6);
  int x = printf("%s", s);
}
// Imported from ESBMC regression/cstd/printf_bug_3
// Needs: --k-induction
// RUN: --k-induction
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^dereference failure: NULL pointer$
// Vereda: verdict none
