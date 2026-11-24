void f(int a, ...) {
  assert(a==1);
}

int main() {
  f(1, 2, 3);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Ellipsis
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
