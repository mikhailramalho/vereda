// #10301: realloc of a pointer no allocator returned is undefined behaviour.
#include <stdlib.h>
int main()
{
  int *q = (int *)1;
  q = realloc(q, sizeof(int));
}
// Imported from ESBMC regression/esbmc/github_10301_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED.*invalid-pointer-freed.*line 6.*dereference failure: invalid pointer freed$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
