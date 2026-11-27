int main()
{
  char buf[4] = "abc";
  char *p = __VERIFIER_nondet_bool() ? "abc" : buf;
  __VERIFIER_assume(p == buf);
  __ESBMC_init_object(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10812_init_object_literal
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
