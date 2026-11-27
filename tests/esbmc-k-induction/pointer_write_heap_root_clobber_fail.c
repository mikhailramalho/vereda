// set writes through d, which may point at p or at a heap cell, so the loop
// moves p even though d is traced to a root: p->state is not a write inside
// one fixed *p, and treating it as one would prove b->state < 5.
#include <stdlib.h>
extern void abort(void);
extern int __VERIFIER_nondet_int(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

struct dev
{
  int state;
};

struct dev *p;

void set(struct dev **x, struct dev *v) { *x = v; }

int main()
{
  struct dev *a = malloc(sizeof(struct dev));
  struct dev *b = malloc(sizeof(struct dev));
  a->state = 0;
  b->state = 0;
  p = a;
  struct dev **d = __VERIFIER_nondet_int() ? &p : malloc(sizeof(struct dev *));
  for (;;)
  {
    set(d, b);
    p->state = p->state + 1;
    __VERIFIER_assert(b->state < 5);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_clobber_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: Disabling inductive step
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
