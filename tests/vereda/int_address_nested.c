// Cancel a base below multiple additions, keeping a symbolic offset.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct port { uint64_t fields[16]; } ports[4];
int main(void) {
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 4);
  uintptr_t base = (uintptr_t)ports + sizeof(struct port) * k;
  *(uint64_t *)(base + 8) = 19;
  __VERIFIER_assert(ports[k].fields[1] == 19);
}
