int main()
{
  unsigned ma, mb;
  unsigned long long accu;
  int ea, eb;

  accu >>= 24;
  accu >>= 1;

  assert(0);
}


// Imported from ESBMC regression/esbmc/00_bitshift_01
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
