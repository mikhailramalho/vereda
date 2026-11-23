// clear() writes through a pointer nothing resolves, but nothing calls it.
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

void clear(int *p, int n)
{
  for (int i = 0; i < n; i++)
    p[i] = 0;
}

int main()
{
  int i = 0;
  for (;;)
  {
    i = (i + 1) % 5;
    __VERIFIER_assert(i < 5);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_unreachable_helper
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
