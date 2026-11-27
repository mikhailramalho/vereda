// The loop writes s.a only through q->a; havocking s still proves the bound.
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

struct s
{
  int a;
  int b;
};

int main()
{
  struct s s = {0, 0};
  struct s *q = &s;
  for (;;)
  {
    q->a = (q->a + 1) % 10;
    __VERIFIER_assert(s.a < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_member
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
