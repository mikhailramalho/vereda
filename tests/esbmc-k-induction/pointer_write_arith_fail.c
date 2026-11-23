// The loop writes a[] only through *(p + k); the inductive step must havoc a.
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int main()
{
  int a[4] = {0, 0, 0, 0};
  int *p = a;
  unsigned k = 0;
  for (;;)
  {
    *(p + k % 4) = *(p + k % 4) + 1;
    k++;
    __VERIFIER_assert(a[0] < 3);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_arith_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
