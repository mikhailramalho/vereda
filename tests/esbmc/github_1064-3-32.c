#include <stdio.h>

int main(int argc, char* argv[]) {
  __ESBMC_alloc_size[__ESBMC_POINTER_OBJECT(argv)] = (size_t)(argc+1)*sizeof(*argv);

  if (argv[0]) {}
  if (argv[argc]) {}
  int i;
  __VERIFIER_assume(0 <= i);
  __VERIFIER_assume(i <= argc);
  if (argv[i]) {}

  return 0;
}
// Imported from ESBMC regression/esbmc/github_1064-3-32
// Needs: __ESBMC_POINTER_OBJECT
// RUN: -m32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
