// The buffer stays reachable through the live node the global points at.
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
  return 0;
}
// Imported from ESBMC regression/esbmc/memleak_reachable_via_live
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
