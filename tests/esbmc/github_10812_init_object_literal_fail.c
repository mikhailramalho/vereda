int main()
{
  char buf[4] = "abc";
  char *p = __VERIFIER_nondet_bool() ? "abc" : buf;
  __ESBMC_init_object(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10812_init_object_literal_fail
// Needs: __ESBMC_init_object
// MODE: FUTURE
// EXPECT: ^  dereference failure: write access to string constant$
// EXPECT: ^VERIFICATION FAILED$
