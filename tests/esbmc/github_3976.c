#include <stdlib.h>

long a;

typedef union {
} b;

typedef struct {
  b c;
} d;

d *e;

int main() {
  void *f = malloc(a);
  e = f;
  e->c;
}
// Imported from ESBMC regression/esbmc/github_3976
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
