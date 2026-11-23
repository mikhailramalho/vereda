int main(void) {}
// Imported from ESBMC regression/k-induction/github_441
// Needs: --k-induction --k-step 4
// RUN: --k-induction --k-step 4 --max-k-step 9
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
