unsigned long __VERIFIER_nondet_ulong(void);
int main(void)
{
  unsigned long a = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(a == 6098646942UL);
  unsigned long r = a << 32;
  return (int)(r >> 63);
}
// Imported from ESBMC regression/esbmc/github_9223_unsigned_signed_amount_fail
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^  !overflow\("shl", a, 32\)$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  !overflow\("shl", a, 32\)$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
