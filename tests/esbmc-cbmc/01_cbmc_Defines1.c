int main() {
#ifdef NEW_DEFINE
  // Ok!
#else
  // define is missing
  assert(0);
#endif
}
// Imported from ESBMC regression/cbmc/01_cbmc_Defines1
// RUN: -DNEW_DEFINE --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
