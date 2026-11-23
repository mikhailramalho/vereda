// irq writes at an index it loads from d's object, which never holds an
// address, so the index is an offset into that object. The inductive step
// havocs d's object whole and proves calls < 3.
#include <stdlib.h>
extern void abort(void);
extern unsigned char __VERIFIER_nondet_uchar(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

struct dev
{
  int n;
  unsigned char buf[8];
};

int calls;

void irq(struct dev *d, unsigned char data)
{
  unsigned long base = (unsigned long)d;
  int n = *(int *)base;
  if (n >= 0 && n < 8)
    *(unsigned char *)(base + 4 + n) = data;
  calls = (calls + 1) % 3;
}

int main()
{
  struct dev *d = malloc(sizeof(struct dev));
  d->n = 0;
  for (;;)
  {
    irq(d, __VERIFIER_nondet_uchar());
    __VERIFIER_assert(calls < 3);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_index_load
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
