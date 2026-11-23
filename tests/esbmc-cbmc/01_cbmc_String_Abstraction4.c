unsigned strlen(const char *);
char *strcpy(char *dest, const char *src);

int main()
{
  char a[100], b[101];

  b[100]=0;

  // this should fail, off by one!
  strcpy(a, b);
}
// Imported from ESBMC regression/cbmc/01_cbmc_String_Abstraction4
// RUN: --unwind 103
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
