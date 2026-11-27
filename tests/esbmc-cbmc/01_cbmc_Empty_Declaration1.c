int blah(void);
; // empty!

int main() {
}
// Imported from ESBMC regression/cbmc/01_cbmc_Empty_Declaration1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
