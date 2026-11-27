unsigned long __VERIFIER_nondet_ulong(void);
int main(void)
{
  unsigned long a = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(a == 3049323471UL);
  unsigned long r = a << 32;
  return (int)(r >> 63);
}
// Imported from ESBMC regression/esbmc/github_9223_unsigned_signed_amount
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
