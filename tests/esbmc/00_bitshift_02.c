int main()
{
  long long accu;

  accu >>= 24;
  accu >>= 1;

  assert(0);
}


// Imported from ESBMC regression/esbmc/00_bitshift_02
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
