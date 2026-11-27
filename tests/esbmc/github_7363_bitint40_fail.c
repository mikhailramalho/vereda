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
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*arithmetic overflow on add$
