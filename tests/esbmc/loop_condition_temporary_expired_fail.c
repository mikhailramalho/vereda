struct S { int a[2]; };
int n;
struct S f() { struct S s = {{n < 3, 0}}; n++; return s; }
int main()
{
  int *q;
  while ((q = f().a)[0])
    __VERIFIER_assert(q[1] == 0); // temporary read in body
  return 0;
}
// Imported from ESBMC regression/esbmc/loop_condition_temporary_expired_fail
// RUN: --no-incremental-bmc
// EXPECT: accessed expired variable pointer `main::\$tmp::return_value\$_f\$1'
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: accessed expired variable pointer `main::\$tmp::return_value\$_f\$1'
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
