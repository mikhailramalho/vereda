// main calls itself, so g stays a global: as a local, the inner call would
// see g == 0 again and recurse past the bound.
// RUN: --unwind 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int g;
int main(void) {
  if (g == 0) {
    g = 1;
    main();
  }
  __VERIFIER_assert(g == 1);
  return 0;
}
