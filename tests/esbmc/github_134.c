#include<stdlib.h>
#include<assert.h>

struct A {
  char alloc;
  int b[];
} a;

int main() {
  struct A *e = calloc(1, sizeof(struct A) + 3*sizeof(int));
  (*e).b[3] = a.b[0];
  assert((*e).b[3] == 0);
  *e;
  free(e);
}
// Imported from ESBMC regression/esbmc/github_134
// EXPECT: (?s)dereference failure.*^VERIFICATION FAILED$
// Vereda: unmet EXPECT: (?s)dereference failure.*^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict FAILED
