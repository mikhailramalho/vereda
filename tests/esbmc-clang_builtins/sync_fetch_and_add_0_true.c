
int main() {
  int a = 42;
  int v = 8;
  int fetch = __sync_fetch_and_add(&a, v);
  __VERIFIER_assert(a == 50); // a1 = a0+v
  __VERIFIER_assert(fetch == 42); // fetch = a0
  return 0;
}
// Imported from ESBMC regression/clang_builtins/sync_fetch_and_add_0_true
// EXPECT: ^VERIFICATION SUCCESSFUL$
