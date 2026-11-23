int main(void)
{
  unsigned char c = 1;
  c <<= 32;
  return c;
}
// Imported from ESBMC regression/esbmc/github_9436_fail
// RUN: --ub-shift-check
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: undefined behavior on shift operation shl$
// Vereda: unmet EXPECT: undefined behavior on shift operation shl$
// MODE: FUTURE
// Vereda: verdict FAILED
