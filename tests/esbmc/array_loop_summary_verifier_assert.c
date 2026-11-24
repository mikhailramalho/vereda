// The usual SV-COMP __VERIFIER_assert keeps no state, so the check loop is
// summarised.
extern void abort(void);
void reach_error() { __VERIFIER_assert(0); } // reach_error
void __VERIFIER_assert(int c)
{
  if (!c)
  {
    reach_error();
    abort();
  }
}
int a[10];
int main()
{
  for (int i = 0; i < 10; i++)
    __VERIFIER_assert(a[i] == 0);
}
// Imported from ESBMC regression/esbmc/array_loop_summary_verifier_assert
// RUN: --summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^Summarised 1 array loop\(s\)$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
