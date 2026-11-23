// The address reaches the callee inside a pointer-free struct passed by value.
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

struct s
{
  unsigned long a;
};

int x;
int *g;

void take(struct s v)
{
  g = (int *)v.a;
}

int main()
{
  struct s s;
  s.a = (unsigned long)&x;
  take(s);
  int *p = g;
  int i = 0;
  for (;;)
  {
    p[0] = p[0] + 1;
    i++;
    __VERIFIER_assert(x < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_aggregate_arg_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
