#include <stdlib.h>

// Passing companion to cwe_excessive_alloc_typed: a typed malloc(sizeof(T))
// whose real byte size (1000) is under the bound must succeed. Guards the typed
// size scaling against over-counting: a spurious extra sizeof factor
// (1000 * 1000) would push the request past the 2000-byte bound and wrongly
// FAIL. Together with cwe_excessive_alloc_typed this brackets the scaling from
// both sides, mirroring the vla / vla_pass pair.
struct Mid
{
  char b[1000];
};

int main(void)
{
  struct Mid *p = malloc(sizeof(struct Mid));
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_excessive_alloc_typed_pass
// Needs: --excessive-alloc-check=2000
// RUN: --excessive-alloc-check=2000 --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
