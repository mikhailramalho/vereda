// The loop writes s.a only through q->a; the inductive step must havoc s.
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
  int i = 0;
  for (;;)
  {
    q->a++;
    i++;
    __VERIFIER_assert(s.a < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_member_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
