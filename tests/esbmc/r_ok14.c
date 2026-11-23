#include <assert.h>

struct Inner {
  char data[5];
};

struct Outer {
  struct Inner in;
};

int main() {
  struct Outer o;
  assert(__ESBMC_r_ok(o.in.data, 5));
}

// Imported from ESBMC regression/esbmc/r_ok14
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
