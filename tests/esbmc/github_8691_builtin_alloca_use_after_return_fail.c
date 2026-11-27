int *f(void)
{
  int *p = __builtin_alloca(sizeof(int));
  *p = 1;
  return p;
}
int main(void)
{
  int *p = f();
  return *p;
}
// Imported from ESBMC regression/esbmc/github_8691_builtin_alloca_use_after_return_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[main\.invalidated-dynamic-object\.1\]\s+line 10\s+dereference failure: invalidated dynamic object$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
