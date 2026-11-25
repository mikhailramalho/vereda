// Large partial __VERIFIER_nondet_memory makes bytes in the range nondet.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
#include <stddef.h>
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
  __VERIFIER_assert(s.buf[i] == 0);
  return 0;
}
