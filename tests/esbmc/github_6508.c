struct S
{
  int buf[4];
  unsigned n;
};

int main()
{
  struct S s;
  struct S *p = &s;
  p->n = 4;
  p->buf[p->n] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6508
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*array bounds violated: array upper bound$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
