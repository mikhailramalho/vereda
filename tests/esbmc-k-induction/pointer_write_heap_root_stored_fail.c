// probe writes g through d->cnt, a pointer loaded from the object d points
// to. Havocing that object whole leaves the loaded pointer unresolved in the
// inductive step, which would drop the write and prove g == before, so the
// step is disabled.
#include <stdlib.h>
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

struct dev
{
  int state;
  int *cnt;
};

int g;

void probe(struct dev *d)
{
  int *c = d->cnt;
  d->state = 1;
  *c = *c + 1;
}

int main()
{
  struct dev *d = malloc(sizeof(struct dev));
  d->cnt = &g;
  for (;;)
  {
    int before = g;
    probe(d);
    __VERIFIER_assert(g == before || g < 5);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_stored_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: Disabling inductive step
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
