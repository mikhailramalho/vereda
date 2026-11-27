int main(void)
{
  unsigned char c = 1;
  c <<= 32;
  return c;
}
// Imported from ESBMC regression/esbmc/github_9436_fail
// RUN: --ub-signed-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: undefined behavior on shift operation shl$
