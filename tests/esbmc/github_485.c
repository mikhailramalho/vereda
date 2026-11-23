int main()
{
  int resp, a, b;

  resp=a+b;

  assert(resp==a+b);
}
// Imported from ESBMC regression/esbmc/github_485
// Needs: --symbol-table-too
// RUN: --overflow-check --symbol-table-too -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
