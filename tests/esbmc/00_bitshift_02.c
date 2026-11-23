int main()
{
  long long accu;

  accu >>= 24;
  accu >>= 1;

  assert(0);
}


// Imported from ESBMC regression/esbmc/00_bitshift_02
// Needs: --bv
// RUN: --bv --no-slice -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
