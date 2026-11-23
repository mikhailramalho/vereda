int main(void) {}
// Imported from ESBMC regression/k-induction/github_441_1
// Needs: --k-induction
// RUN: --k-induction --k-step 9 --max-k-step 4
// MODE: FUTURE
// EXPECT: ^ERROR: --k-step \(9\) must be smaller than --max-k-step \(4\)\.
// EXPECT: ^ESBMC version
