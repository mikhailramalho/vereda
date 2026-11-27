// Regression for GitHub #7840 (signedness counterpart): both operands are
// widened by exactly the safe width (32 + 32 == 64), but from unsigned
// sources into a signed destination. The no-overflow shortcut requires the
// source and destination signedness to match, so it must not fire here, and
// the genuine overflow (e.g. two large unsigned values) must still be caught.
long long p(unsigned a, unsigned b)
{
  return (long long)a * (long long)b;
}
// Imported from ESBMC regression/esbmc/github_7840_signedness_fail
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --entry p --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
