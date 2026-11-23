// #9883: a ternary is located at its `?`, not where the expression starts.
extern int __VERIFIER_nondet_int(void);
int f(void) { return 2; }

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = (x > 5)
    ? f()
    : 0;
  __VERIFIER_assert(y == 0); // reached
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/ternary_start_line_9883
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
