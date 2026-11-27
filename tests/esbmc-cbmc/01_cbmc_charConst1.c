int main ()
{
  char x = '\n';
  char y = '\0';
  char z = '\1';
  char o = '\144';
  assert (x == 10);
  assert (y == 0);
  assert (z == 1);
  assert (o == 100);
}
// Imported from ESBMC regression/cbmc/01_cbmc_charConst1
// EXPECT: ^VERIFICATION SUCCESSFUL$
