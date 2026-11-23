int main()
{
  int a[5];
  unsigned n = (unsigned)((char *)&a[4] - (char *)&a[0]);
  int s = 0;
  for (unsigned i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(s == 16); // four ints is sixteen bytes
  return 0;
}
// Imported from ESBMC regression/esbmc/byte_address_difference
// RUN: --unwind 20
// EXPECT: ^VERIFICATION SUCCESSFUL$
