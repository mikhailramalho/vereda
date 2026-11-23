int __VERIFIER_nondet_int();
int main()
{
  int x = __VERIFIER_nondet_int();
  int r = 0;
  while (__VERIFIER_nondet_int())
  {
    int t[2] = {x, 0};
    r = t[1];
  }
  return r;
}
// Imported from ESBMC regression/esbmc/loop_local_array_l1_reuse
// Needs: --program-only
// RUN: --unwind 3 --no-unwinding-assertions --program-only --no-slice
// MODE: FUTURE
// EXPECT: \bt\?1!0&0#\d+ == \{
// EXPECT: \A(?![\s\S]*\bt\?(?!1!)\d)
// Vereda: verdict none
