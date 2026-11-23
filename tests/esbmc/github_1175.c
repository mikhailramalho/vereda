int main()
{
  int *ptr = (int*)90;
  int v = *ptr;
}
// Imported from ESBMC regression/esbmc/github_1175
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
