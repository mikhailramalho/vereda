unsigned strlen(const char *);
char *strcpy(char *dest, const char *src);

int main()
{
  char a[100], b[101];

  b[100] = 0;

  // this should fail, off by one!
  strcpy(a, b);
}
// Imported from ESBMC regression/incremental-smt/incremental-16
// Needs: --smt-symex-assert --smt-symex-guard
// RUN: --smt-symex-assert --smt-symex-guard --z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
