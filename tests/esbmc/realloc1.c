#include <stdlib.h>

int main() {
  int *p = NULL;
  p = realloc(p, 10 * sizeof(int)); // should behave like malloc
  if (p == NULL) return 0; // allocation may fail
  p[0] = 42;               // must be safe if allocation succeeded
  free(p);
  return 0;
}

// Imported from ESBMC regression/esbmc/realloc1
// EXPECT: ^VERIFICATION SUCCESSFUL$
