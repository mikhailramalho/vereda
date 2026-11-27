// The first loop assigns d, so its inductive step havocs d and the second
// loop starts with a pointer symex cannot resolve: a havoc through d there
// reaches nothing and the write bump makes is dropped, which would prove
// b->state < 5. d is no root.
#include <stdlib.h>
extern void abort(void);
extern int __VERIFIER_nondet_int(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

struct dev
{
  int state;
};

void bump(struct dev *d) { d->state = d->state + 1; }
struct dev *same(struct dev *d) { return d; }

int main()
{
  struct dev *b = malloc(sizeof(struct dev));
  b->state = 0;
  struct dev *d = b;
  while (__VERIFIER_nondet_int())
    d = same(d);
  for (;;)
  {
    bump(d);
    __VERIFIER_assert(b->state < 5);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_looped_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: Disabling inductive step
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
