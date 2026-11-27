typedef struct {
  int a
} b;
b *c;
main() {
  c = malloc(sizeof(b));
  c->a || d();
}
// Imported from ESBMC regression/esbmc/github_387_false
// Needs: --memory-leak-check
// RUN: --memory-leak-check --z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
