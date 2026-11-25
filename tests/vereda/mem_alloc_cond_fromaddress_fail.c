// On the path where a conditional constant-size allocation executes, an
// integer address equal to its base still converts back to that object.
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>
#include <stdlib.h>

extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
extern uintptr_t __VERIFIER_nondet_ulong(void);

int main(void) {
  if (__VERIFIER_nondet_int()) {
    int *p = malloc(sizeof(int));
    if (p) {
      *p = 42;
      uintptr_t a = __VERIFIER_nondet_ulong();
      int *r = (int *)a;
      if (r == p)
        __VERIFIER_assert(*r != 42);
    }
  }
  return 0;
}
