#include <stdlib.h>

typedef union {
} b;

typedef struct {
  b c;
  int a;
} d;

d *e;

int main() {
  void *f = malloc(sizeof(d));
  e = f;
  e->c;
}
// Imported from ESBMC regression/esbmc/github_3976_2
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
