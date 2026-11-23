typedef struct {
  int a
} b;
b *c;
main() {
  c = malloc(sizeof(b));
  c->a || d();
  free(c);
}
// Imported from ESBMC regression/esbmc/github_387_true
// Needs: --memory-leak-check
// RUN: --no-slice --memory-leak-check --z3 --force-malloc-success -Wno-error=implicit-function-declaration -Wno-error=implicit-int
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
