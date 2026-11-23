int main()
{
  int var;
  unsigned long int ptr;
  int *c = (int *)ptr;
  *c = 42;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_382_5
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
