void __ESBMC_havoc_object(void *);
char c;
int main(void)
{
  char *s = __VERIFIER_nondet_int() ? &c : "lit";
  __ESBMC_havoc_object(s);
  return 0;
}
// Imported from ESBMC regression/esbmc/havoc_object_string_literal_fail
// RUN: --incremental-bmc
// EXPECT: ^  dereference failure: write access to string constant$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: write access to string constant$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
