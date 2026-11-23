#include <stdlib.h>

struct S
{
  int buf[4];
  unsigned n;
};

int main()
{
  /* Indexing a real array selects a fixed-size element, so `buf' stays
     interior and keeps its declared bound even though the array itself was
     reached through a pointer. */
  struct S *a = malloc(4 * sizeof(struct S));
  if (!a)
    return 0;

  a[1].buf[6] = 1;
  free(a);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6508-arrayelem
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*array bounds violated: array upper bound$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
