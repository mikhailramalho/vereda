// f redirects its parameter through the parameter's own address, so binding
// the call's argument to it would miss the write to y.
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int x, y;

void f(int *q)
{
  int **pp = &q;
  *pp = &y;
  *q = *q + 1;
}

int main()
{
  int i = 0;
  for (;;)
  {
    f(&x);
    i++;
    __VERIFIER_assert(y < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_param_aliased_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
