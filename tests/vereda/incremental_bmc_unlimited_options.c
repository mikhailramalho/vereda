// --unlimited-k-steps and --k-step mean nothing without --incremental-bmc.
// RUN: --unlimited-k-steps
// EXPECT: --unlimited-k-steps needs --incremental-bmc
// EXPECT-NOT: VERIFICATION
int main(void) { return 0; }
