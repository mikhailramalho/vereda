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
// Needs: --smt-during-symex --smt-symex-assert --smt-symex-guard --add-symex-value-sets
// RUN: --smt-during-symex --smt-symex-assert --smt-symex-guard --add-symex-value-sets --z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
