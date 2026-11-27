struct S
{
  int buf[4];
  unsigned n;
};

int main()
{
  struct S s;
  struct S *p = &s;

  p->n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(p->n <= 4);

  /* n == 4 writes onto `n' itself, which stays inside the enclosing struct:
     only the array's own bound rejects it. */
  p->buf[p->n] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6508-nondet
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*array bounds violated: array upper bound$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
