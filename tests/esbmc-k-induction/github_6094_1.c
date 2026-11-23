int main(void) {}
// Imported from ESBMC regression/k-induction/github_6094_1
// Needs: --k-induction --base-k-step 1
// RUN: --k-induction --base-k-step 1 --max-k-step 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
