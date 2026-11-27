// Peephole identities from #626. Each must hold for every input, including at
// the wrapping boundaries, so the assertions below are the semantics the folds
// have to preserve rather than a check that they fired.
int __VERIFIER_nondet_int(void);
unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();

  __VERIFIER_assert(x + x == (x << 1)); // x + x is x << 1
  __VERIFIER_assert(~b + 1 == -b); // ~b + 1 is -b
  __VERIFIER_assert((a + 1) + ~b == a - b); // (a + 1) + ~b is a - b
  __VERIFIER_assert(~b + (a + 1) == a - b); // ~b + (a + 1) is a - b
  __VERIFIER_assert((a + ~b) + 1 == a - b); // (a + ~b) + 1 is a - b
  __VERIFIER_assert((~b + a) + 1 == a - b); // (~b + a) + 1 is a - b
  __VERIFIER_assert(~b + 3 == 2 - b); // ~b + C is (C - 1) - b

  // The same identities on an unsigned type, where the wrap is the point.
  unsigned u = __VERIFIER_nondet_uint();
  unsigned v = __VERIFIER_nondet_uint();
  __VERIFIER_assert(u + u == (u << 1)); // unsigned x + x is x << 1
  __VERIFIER_assert(~v + 1u == -v); // unsigned ~v + 1 is -v
  __VERIFIER_assert((u + 1u) + ~v == u - v); // unsigned (u + 1) + ~v is u - v

  return 0;
}
// Imported from ESBMC regression/esbmc/add_peephole_identities
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
