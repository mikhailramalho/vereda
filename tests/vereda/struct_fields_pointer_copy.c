// RUN: --no-summarize-array-loops --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct pointer_slot { int *p; unsigned tag; };
struct integer_slot { uintptr_t bits; unsigned tag; };
int main(void) {
  int target = 7;
  struct pointer_slot a[128], b[128];
  struct integer_slot bytes[128];
  for (unsigned i = 0; i < 128; ++i) {
    a[i].p = &target;
    a[i].tag = i;
  }
  memcpy(bytes, a, sizeof a);
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 128);
  __VERIFIER_assert(bytes[i].bits == (uintptr_t)&target);
  __VERIFIER_assert(bytes[i].tag == i);
  memcpy(b, bytes, sizeof b);
  __VERIFIER_assert(b[i].p == &target);
  __VERIFIER_assert(*b[i].p == 7);
  __VERIFIER_assert(b[i].tag == i);
}
