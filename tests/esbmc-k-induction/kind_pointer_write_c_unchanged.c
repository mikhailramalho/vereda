#include <assert.h>
#include <stdlib.h>

struct list
{
  unsigned size;
  int *slot;
};

unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  struct list *l = malloc(sizeof(struct list));
  if (!l)
    return 0;
  l->size = 0;
  l->slot = malloc(16 * sizeof(int));
  if (!l->slot)
    return 0;
  unsigned n = __VERIFIER_nondet_uint();
  for (unsigned i = 0; i < n; i++)
  {
    l->slot[i % 16] = 1;
    l->size++;
  }
  assert(l->size == n);
  return 0;
}
// Imported from ESBMC regression/k-induction/kind_pointer_write_c_unchanged
// Needs: --k-induction
// RUN: --k-induction --max-k-step 4
// MODE: FUTURE
// EXPECT: ^WARNING: k-induction cannot havoc what a loop writes through a pointer. Disabling inductive step$
// Vereda: verdict none
