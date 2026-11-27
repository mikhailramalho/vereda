// --max-k-step means nothing without incremental BMC.
// RUN: --max-k-step 3 --no-incremental-bmc
// EXPECT: --max-k-step needs incremental BMC, which --no-incremental-bmc
// EXPECT-NOT: VERIFICATION
int main(void) { return 0; }
