// An ite join over two partial-fill lambdas followed by reads reduces each
// arm directly to ite(c, 3, 5) instead of constraining lambda array symbols.
// RUN: --unwind 514
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
struct S {
  int head;
  unsigned char buf[1024];
};
int main(void) {
  struct S s;
  s.head = 7;
  if (__VERIFIER_nondet_int())
    memset(s.buf, 3, 1024);
  else
    memset(s.buf, 5, 1024);
  unsigned sum = 0;
  for (int i = 0; i < 512; ++i)
    sum += s.buf[i];
  __VERIFIER_assert((sum == 512u * 3u || sum == 512u * 5u) && s.head == 7);
  return 0;
}
