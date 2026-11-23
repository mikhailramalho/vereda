// inc writes x through its parameter, resolved in the callee's scope.
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int x;
void inc(int *q) { *q = *q + 1; }

int main()
{
  int i = 0;
  for (;;)
  {
    inc(&x);
    i++;
    __VERIFIER_assert(x < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_callee_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
