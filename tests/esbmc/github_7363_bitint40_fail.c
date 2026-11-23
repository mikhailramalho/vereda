int main()
{
  unsigned _BitInt(40) um = 0;
  um = ~um;
  um = um >> 1;
  signed _BitInt(40) m = (signed _BitInt(40))um;
  signed _BitInt(40) one = 1;
  signed _BitInt(40) r = m + one;
  return (int)r;
}
// Imported from ESBMC regression/esbmc/github_7363_bitint40_fail
// Needs: REQUIRES bitint
// RUN: --overflow-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*arithmetic overflow on add$
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
