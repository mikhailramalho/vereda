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
// RUN: --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
