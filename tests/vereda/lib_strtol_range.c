// strtoll and strtoull at the edges of their range: a last digit past
// LLONG_MAX, LLONG_MIN or ULLONG_MAX gives the limit and sets errno to ERANGE.
// RUN: --unwind 24
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char c = __VERIFIER_nondet_char();
  int dig = c >= '0' && c <= '9';
  char *e;
  char s[20] = "922337203685477580";
  s[18] = c;
  errno = 0;
  long long v = strtoll(s, &e, 10);
  __VERIFIER_assert(v == (!dig ? 922337203685477580LL
                          : c <= '7' ? 9223372036854775800LL + (c - '0') : LLONG_MAX));
  __VERIFIER_assert(errno == (dig && c >= '8' ? ERANGE : 0));
  __VERIFIER_assert(e == (dig ? s + 19 : s + 18));

  char n[21] = "-922337203685477580";
  n[19] = c;
  errno = 0;
  v = strtoll(n, &e, 10);
  __VERIFIER_assert(v == (!dig ? -922337203685477580LL
                          : c <= '8' ? -9223372036854775800LL - (c - '0') : LLONG_MIN));
  __VERIFIER_assert(errno == (c == '9' ? ERANGE : 0));

  char u[21] = "1844674407370955161";
  u[19] = c;
  errno = 0;
  unsigned long long w = strtoull(u, &e, 10);
  __VERIFIER_assert(w == (!dig ? 1844674407370955161ULL
                          : c <= '5' ? 18446744073709551610ULL + (c - '0') : ULLONG_MAX));
  __VERIFIER_assert(errno == (dig && c >= '6' ? ERANGE : 0));
  return 0;
}
