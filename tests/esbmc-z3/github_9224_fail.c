// #9224: an arithmetic shift rounds toward minus infinity, so it differs
// from signed division by 2^k on negative operands.
int __VERIFIER_nondet_int(void);
int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x / 4 == x >> 2); // x / 4
  return 0;
}
// Imported from ESBMC regression/z3/github_9224_fail
// RUN: --solver z3 --no-incremental-bmc
// EXPECT: ^\s+x = -[0-9]+
// EXPECT: ^VERIFICATION FAILED$
