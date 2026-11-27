int __VERIFIER_nondet_int(void);

struct
{
  int a;
} b = {3};

int main()
{
  // The same arithmetic on a non-bitfield member is checked; pins the boundary
  // rather than --overflow-check as a whole.
  int a = __VERIFIER_nondet_int();
  b.a += a;
}
// Imported from ESBMC regression/esbmc/overflow_plain_member
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
