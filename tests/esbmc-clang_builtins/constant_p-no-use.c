const int a = 42;

int main() {
  __builtin_constant_p(a);
  return 0;
}
// Imported from ESBMC regression/clang_builtins/constant_p-no-use
// EXPECT: ^VERIFICATION SUCCESSFUL$
