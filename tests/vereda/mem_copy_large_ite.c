// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern _Bool __VERIFIER_nondet_bool(void);
extern void __VERIFIER_assume(int);

struct Big {
  long long a[2048];
};

int main(void) {
  struct Big s1, s2, dst;
  memset(&s1, 0, sizeof(s1));
  int j = __VERIFIER_nondet_int();
  __VERIFIER_assume(j >= 0 && j < 2048);
  s2.a[j] = 42;
  _Bool b = __VERIFIER_nondet_bool();
  if (b)
    memcpy(&dst, &s1, sizeof(dst));
  else
    memcpy(&dst, &s2, sizeof(dst));
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2048);
  if (b)
    assert(dst.a[i] == 0);
  else if (i == j)
    assert(dst.a[i] == 42);
  return 0;
}
