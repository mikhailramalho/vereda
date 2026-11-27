// As pointer_write_heap_root_reassigned_fail, with the loop entered by a jump
// to its test at the bottom.
#include <stdlib.h>
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
_Bool __VERIFIER_nondet_bool(void);

struct dev
{
  int state;
};

void *alloc(unsigned n) { return malloc(n); }

int main()
{
  alloc(1);
  struct dev *d = alloc(sizeof(struct dev));
  d->state = 0;
  unsigned long base;
  unsigned long field;
  goto test;
body:
  base = (unsigned long)d;
  field = base + 0;
  *(int *)field = d->state + 1;
  __VERIFIER_assert(d->state < 5);
test:
  if (__VERIFIER_nondet_bool())
    goto body;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_reassigned_entry_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
