// Check if the flag don't disable other pointer checks
int main(void) {
    int* p = (int*) malloc(sizeof(int) *10); // --assume-malloc-success
    int q;
    return p[q];
}
// Imported from ESBMC regression/esbmc/no_pointer_check_1_fail
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
