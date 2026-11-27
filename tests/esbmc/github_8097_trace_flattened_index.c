/* #8097: an element of a two-dimensional array read back from a union's bits
 * for the counterexample is selected row by row, not by its flattened index. */
int __VERIFIER_nondet_int();
int main()
{
  union
  {
    unsigned long long l[6];
    int m[3][4];
  } u;
  u.m[0][2] = __VERIFIER_nondet_int();
  u.m[1][2] = __VERIFIER_nondet_int();
  __VERIFIER_assert(u.m[0][2] != 3 || u.m[1][2] != 5); // show
}
// Imported from ESBMC regression/esbmc/github_8097_trace_flattened_index
// RUN: --no-incremental-bmc
// EXPECT: ^  u\.m\[0\]\[2\] = 3
// EXPECT: ^  u\.m\[1\]\[2\] = 5
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  u\.m\[0\]\[2\] = 3
// MODE: FUTURE
// Vereda: verdict FAILED
