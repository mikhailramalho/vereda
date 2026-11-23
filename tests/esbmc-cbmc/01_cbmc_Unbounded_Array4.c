#include <stdint.h>

// Previously this code, which selects and updates idx 0 of asdfasdf at the
// same time, was causing trouble in the array flattener as it'd overwrite the
// select with the update (5564f93e)

struct foo {
  uint32_t bar;
  uint32_t baz;
};

struct foo asdfasdf[200000];

int main() {
  struct foo xyzzy;

  asdfasdf[0].bar = 1;
  asdfasdf[0].baz = 1;

  uint32_t *fuzz;
  if (__VERIFIER_nondet_bool()) {
    fuzz = &asdfasdf[0].bar;
  } else {
    fuzz = &asdfasdf[0].baz;
  }

  uint32_t end = *fuzz;
  assert(end == 0 || end == 1);
  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Unbounded_Array4
// Needs: --array-flattener
// RUN: --array-flattener -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
