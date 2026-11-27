// free takes only what an allocator returned (C17 7.22.3.3p2): here a
// local, a global or a string literal, chosen by nondet values, each a
// free of non-dynamic memory as ESBMC words it.
// EXPECT: ^  dereference failure: free\(\) of non-dynamic memory$
// EXPECT: ^  FAILED       \[main\.free-non-dynamic-memory\.1\]  line 17  dereference failure: free\(\) of non-dynamic memory$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>

int __VERIFIER_nondet_int(void);

int g;

int main(void) {
  int x;
  int n = __VERIFIER_nondet_int();
  void *p = n == 0 ? (void *)&x : n == 1 ? (void *)&g : (void *)"literal";
  free(p);
  return 0;
}
