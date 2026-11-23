#include <string.h>
int main()
{
  char *p = "abc";
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n <= 3);
  memcpy(p, "xyz", n);
  return 0;
}
// Imported from ESBMC regression/esbmc/string_literal_write_memcpy_symbolic_fail
// EXPECT: ^  dereference failure: write access to string constant$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: write access to string constant$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
