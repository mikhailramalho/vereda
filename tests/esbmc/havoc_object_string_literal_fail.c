void __ESBMC_havoc_object(void *);
char c;
int main(void)
{
  char *s = __VERIFIER_nondet_int() ? &c : "lit";
  __ESBMC_havoc_object(s);
  return 0;
}
// Imported from ESBMC regression/esbmc/havoc_object_string_literal_fail
// Needs: __ESBMC_havoc_object
// RUN: --incremental-bmc
// MODE: FUTURE
// EXPECT: ^  dereference failure: write access to string constant$
// EXPECT: ^VERIFICATION FAILED$
