int main() {
  int expected = __VERIFIER_nondet_int();
  int actual = __builtin_nontemporal_load(&expected);
  expected++;
  __VERIFIER_assert(expected == actual); // check
  return 0;
}
// Imported from ESBMC regression/clang_builtins/nontemporal_load_int_false
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
