// #10188: a pointer whose value set is unknown can be NULL, and dereferencing
// it there is a NULL dereference.
extern void *__VERIFIER_nondet_pointer(void);

int main(void)
{
  int *d = __VERIFIER_nondet_pointer();
  if (d == 0)
    return *d;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10188_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  dereference failure: NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: NULL pointer$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
