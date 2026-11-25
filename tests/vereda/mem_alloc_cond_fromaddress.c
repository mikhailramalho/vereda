// Conditional constant-size allocations get their layout unconditionally,
// while still checking exists in fromAddress() and keeping null conversions
// exact on untaken branches.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
#include <stdlib.h>

extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
extern uintptr_t __VERIFIER_nondet_ulong(void);

int *global_slot;

int main(void) {
  int c = __VERIFIER_nondet_int();
  int *p = 0;
  if (c) {
    p = malloc(sizeof(int));
    if (p) {
      *p = 7;
      global_slot = p;
    }
  }
  if (!c) {
    uintptr_t a = __VERIFIER_nondet_ulong();
    int *r = (int *)a;
    if (a == 0)
      __VERIFIER_assert(r == 0);
    else
      __VERIFIER_assert(r != 0);
    __VERIFIER_assert(global_slot == 0);
  }
  return 0;
}
