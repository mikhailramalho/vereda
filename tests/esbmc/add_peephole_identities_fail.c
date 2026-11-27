// Neighbours of the #626 identities that do not hold. If a fold ever rewrites
// x + x to the wrong shift, or drops the -1 from ~B == -B - 1, one of these
// stops being refutable.
int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  __VERIFIER_assert(x + x == (x << 2)); // x + x is not x << 2
  __VERIFIER_assert(~b + 1 == b); // ~b + 1 is not b
  return 0;
}
// Imported from ESBMC regression/esbmc/add_peephole_identities_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
