int main()
{
  long long accu;
  unsigned char x1=24, x2=1;

  accu >>= x1;
  accu >>= x2;

  assert(0);
}


// Imported from ESBMC regression/esbmc/00_bitshift_03
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
