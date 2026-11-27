int main() {
    double x = -1e11;
    return 0;
}
// Imported from ESBMC regression/esbmc/overflow_float
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
