int main(void) {}
// Imported from ESBMC regression/k-induction/github_6094
// Needs: --k-induction --base-k-step 2
// RUN: --k-induction --base-k-step 2 --max-k-step 2
// MODE: FUTURE
// EXPECT: ^ERROR: --base-k-step \(2\) must be smaller than --max-k-step \(2\)\.
// EXPECT: ^ESBMC version
// Vereda: verdict none
