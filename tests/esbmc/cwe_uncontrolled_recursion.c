// A recursive function with no reachable base case: every path to a return
// goes through the recursive self-call, so the recursion is genuinely
// unbounded (CWE-674 Uncontrolled Recursion) rather than merely deep.
int f(int n)
{
  return f(n + 1);
}

int main(void)
{
  return f(0);
}
// Imported from ESBMC regression/esbmc/cwe_uncontrolled_recursion
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  uncontrolled recursion in f$
// EXPECT: ^  CWE: CWE-674$
// Vereda: unmet EXPECT: ^  uncontrolled recursion in f$
// MODE: FUTURE
// Vereda: verdict FAILED
