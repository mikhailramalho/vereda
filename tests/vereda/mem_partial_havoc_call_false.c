// A partial havoc lambda over a provenance array containing a pointer must
// still report that pointer to havocCall via provBelow.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
#include <stddef.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_nondet_memory(void *, size_t);
extern unsigned __VERIFIER_nondet_uint(void);
struct S {
  int *p;
  unsigned char buf[2048];
};
extern void ext(struct S *);
int main(void) {
  int x = 1;
  struct S s;
  s.p = &x;
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < sizeof(s.buf));
  // Flush s into the arrays so s.p's provenance is in below.prov.
  unsigned char v = s.buf[i];
  (void)v;
  __VERIFIER_nondet_memory(s.buf, sizeof(s.buf));
  ext(&s);
  __VERIFIER_assert(x == 1);
  return 0;
}
