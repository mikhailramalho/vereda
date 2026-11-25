// Large partial __VERIFIER_nondet_memory uses a partial-range lambda: bytes
// outside the range are preserved, reads at the same index agree, and sizes
// past MaxBytes succeed.
// RUN: --unwind 2 --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stddef.h>
#include <stdlib.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_nondet_memory(void *, size_t);
extern unsigned __VERIFIER_nondet_uint(void);
struct S {
  int before;
  unsigned char buf[4096];
  int after;
};
int main(void) {
  struct S s = { 11, {0}, 22 };
  __VERIFIER_nondet_memory(s.buf, sizeof(s.buf));
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < sizeof(s.buf));
  unsigned char x = s.buf[i];
  unsigned char y = s.buf[i];
  __VERIFIER_assert(s.before == 11 && s.after == 22 && x == y);
  unsigned char *p = malloc(2u << 20);
  p[0] = 7;
  __VERIFIER_nondet_memory(p + 1, (2u << 20) - 1);
  __VERIFIER_assert(p[0] == 7);
  return 0;
}
