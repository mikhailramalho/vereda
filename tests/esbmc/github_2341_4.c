#include <stdlib.h>
#include <assert.h>

int main() {
  int *ptr = (int*)malloc(10 * sizeof(int));
  if (ptr == NULL)
    return -1;
  assert(ptr != NULL);

  int *temp = (int*)realloc(ptr, 0); // Should free ptr and return NULL
  if (temp != NULL)
    return -1;
  assert(temp == NULL);

  free(temp); // Freeing NULL is safe
  return 0;
}

// Imported from ESBMC regression/esbmc/github_2341_4
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
