// ctime reflects its timestamp argument; asserting a fixed date for a nondet
// timestamp must fail.
// EXPECT: ^VERIFICATION FAILED$
#include <time.h>
extern void __VERIFIER_assert(int);
extern long __VERIFIER_nondet_long(void);

int main(void) {
  time_t t = __VERIFIER_nondet_long();
  char buf[26];
  ctime_r(&t, buf);
  __VERIFIER_assert(buf[0] == 'T' && buf[1] == 'h' && buf[2] == 'u');
  return 0;
}
