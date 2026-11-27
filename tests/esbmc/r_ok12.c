#include <assert.h>

struct S {
  int x;
  char buf[10];
};

int main() {
  struct S s;
  assert(__ESBMC_r_ok(s.buf, 20)); // out of bounds of struct member
}

// Imported from ESBMC regression/esbmc/r_ok12
// Needs: __ESBMC_r_ok
// RUN: --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
