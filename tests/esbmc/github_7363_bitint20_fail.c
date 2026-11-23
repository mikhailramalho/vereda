int main()
{
  unsigned _BitInt(20) um = 0;
  um = ~um;
  um = um >> 1;
  signed _BitInt(20) m = (signed _BitInt(20))um;
  signed _BitInt(20) one = 1;
  signed _BitInt(20) r = m + one;
  return (int)r;
}
// Imported from ESBMC regression/esbmc/github_7363_bitint20_fail
// Needs: REQUIRES bitint
// RUN: --overflow-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*arithmetic overflow on add$
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
