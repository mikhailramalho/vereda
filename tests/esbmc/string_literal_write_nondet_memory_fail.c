void __VERIFIER_nondet_memory(void *, __SIZE_TYPE__);
int main()
{
  char *p = "abc";
  __VERIFIER_nondet_memory(p, 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/string_literal_write_nondet_memory_fail
// EXPECT: ^  dereference failure: write access to string constant$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: write access to string constant$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
