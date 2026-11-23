void *f(void)
{
  void *p;
  p = __builtin_alloca(sizeof(int));
  *(int *)p = 1;
  return p;
}
int main(void)
{
  int *p = f();
  return *p;
}
// Imported from ESBMC regression/esbmc/github_8691_builtin_alloca_assign_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[main\.invalidated-dynamic-object\.1\]\s+line 11\s+dereference failure: invalidated dynamic object$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
