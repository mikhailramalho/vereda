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
  __VERIFIER_assert(g.flag != 42); // flag can be havocked to 42
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9784_nondet_memory_whole_object_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Symex completed in: [0-9.]+s \([0-9]{1,3} assignments\)$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict FAILED
