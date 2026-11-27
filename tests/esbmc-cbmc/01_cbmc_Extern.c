extern int test;

int test = 0;

int main() {
}
// Imported from ESBMC regression/cbmc/01_cbmc_Extern
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
