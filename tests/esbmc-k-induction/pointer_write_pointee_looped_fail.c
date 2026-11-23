// The first loop assigns d, so its inductive step havocs d and the second
// loop starts with a pointer symex cannot resolve: havocing *d there reaches
// nothing and the write to d->state is dropped, which would prove
// b->state < 5.
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
    d->state = d->state + 1;
    __VERIFIER_assert(b->state < 5);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_pointee_looped_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: Disabling inductive step
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
