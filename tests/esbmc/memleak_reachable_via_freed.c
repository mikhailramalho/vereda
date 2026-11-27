// The global still points at the freed node, but a pointer stored in freed
// memory reaches nothing, so the buffer it held is leaked.
#include <stdlib.h>

struct node
{
  char *buf;
};

struct node *global;

int main()
{
  global = malloc(sizeof(struct node));
  if (!global)
    return 0;
  global->buf = malloc(8);
  free(global);
  return 0;
}
// Imported from ESBMC regression/esbmc/memleak_reachable_via_freed
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory: dynamic_2_array
// Vereda: verdict none
