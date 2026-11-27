#include <assert.h>

struct Inner {
  char data[5];
};

struct Outer {
  struct Inner in;
};

int main() {
  struct Outer o;
  assert(__ESBMC_r_ok(o.in.data, 4));
}

// Imported from ESBMC regression/esbmc/r_ok13
// Needs: __ESBMC_r_ok
// RUN: --ub-pointer-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
