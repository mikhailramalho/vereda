// --unlimited-k-steps and --k-step mean nothing with a bound given.
// RUN: --unlimited-k-steps --unwind 2
// EXPECT: --unlimited-k-steps needs incremental BMC, which .* --unwind
// EXPECT-NOT: VERIFICATION
int main(void) { return 0; }
