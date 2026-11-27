// A double can hold the address too; nothing tracks it, so the write must
// disable the inductive step rather than go unhavocked.
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int x;

int main()
{
  double d = (double)(unsigned long)&x;
  int *p = (int *)(unsigned long)d;
  int i = 0;
  for (;;)
  {
    p[0] = p[0] + 1;
    i++;
    __VERIFIER_assert(x < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_float_launder_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
