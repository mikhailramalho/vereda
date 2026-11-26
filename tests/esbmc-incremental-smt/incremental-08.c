int main()
{
  unsigned int x, y = 0;
  for(x = 0; x < 100; x++)
  {
    y = y + x;
    if(y > 10000)
      assert(0);
  }
  return 0;
}
// Imported from ESBMC regression/incremental-smt/incremental-08
// RUN: -Wno-error=implicit-function-declaration
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
