#include <stddef.h>
extern void __VERIFIER_nondet_memory(void *dst, size_t size);
typedef struct
{
  unsigned long a[512];
  int flag;
} big_t;
big_t g;
int main(void)
{
  __VERIFIER_nondet_memory(&g, sizeof(big_t));
  g.flag = 7;
  __VERIFIER_assert(g.flag == 7); // assigned flag survives

  big_t h;
  h.a[0] = 1;
  __VERIFIER_nondet_memory(&h.a[1], sizeof(unsigned long));
  __VERIFIER_assert(h.a[0] == 1); // bytes outside the range are untouched
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9784_nondet_memory_whole_object
// RUN: --ub-pointer-check=false --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^Symex completed in: [0-9.]+s \([0-9]{1,3} assignments\)$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
