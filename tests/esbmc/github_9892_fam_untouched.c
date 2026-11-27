/* #9892: the flexible array member gives `g` a value-set entry with suffix
 * ".fam[].p", and enumerating the pointers it names crashed. */
#include <stdlib.h>

struct slot
{
  void *p;
};

struct info
{
  void *keep;
  struct slot fam[];
};

struct info g;

int main()
{
  g.keep = malloc(4);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9892_fam_untouched
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
