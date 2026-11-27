// bump writes through d->count, a pointer loaded from memory, not one
// computed from d: havocing the object d points to does not reach *c, and
// would prove *c < 5.
#include <stdlib.h>
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

struct dev
{
  int *count;
};

void bump(struct dev *d) { *d->count = *d->count + 1; }

int main()
{
  struct dev *d = malloc(sizeof(struct dev));
  d->count = malloc(sizeof(int));
  *d->count = 0;
  int *c = d->count;
  for (;;)
  {
    bump(d);
    __VERIFIER_assert(*c < 5);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_load_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --force-malloc-success
// MODE: FUTURE
// EXPECT: Disabling inductive step
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
