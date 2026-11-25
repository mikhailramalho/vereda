// Joining states with multiple separate objects: unchanged objects stay
// shared, one-sided and two-sided updates merge accurately, and an object
// separated on one branch only merges with the other side's shared array.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>

extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  static unsigned char unchanged[1024];
  static unsigned char one_side[1024];
  static unsigned char both_sides[1024];
  static unsigned char become_sep[1024];

  memset(unchanged, 5, sizeof(unchanged));
  memset(one_side, 10, sizeof(one_side));
  memset(both_sides, 20, sizeof(both_sides));

  unsigned idx = __VERIFIER_nondet_uint();
  __VERIFIER_assume(idx < 1024);
  int c = __VERIFIER_nondet_int();

  if (c) {
    one_side[idx] = 11;
    both_sides[idx] = 21;
    memset(become_sep, 31, sizeof(become_sep));
  } else {
    both_sides[idx] = 22;
    become_sep[idx] = 32;
  }

  __VERIFIER_assert(unchanged[idx] == 5);
  __VERIFIER_assert(one_side[idx] == (c ? 11 : 10));
  __VERIFIER_assert(both_sides[idx] == (c ? 21 : 22));
  __VERIFIER_assert(become_sep[idx] == (c ? 31 : 32));
  return 0;
}
