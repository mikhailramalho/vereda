int main(void) {}
// Imported from ESBMC regression/k-induction/github_441_2
// Needs: --k-induction
// RUN: --k-induction --k-step 4 --max-k-step 4
// MODE: FUTURE
// EXPECT: ^ERROR: --k-step \(4\) must be smaller than --max-k-step \(4\)\.
// EXPECT: ^ESBMC version
// Vereda: verdict none
